#include "igneous/engine/Engine.hpp"
#include "igneous/engine/Time.hpp"
#include "igneous/engine/Vec2.hpp"
#include "igneous/input/Input.hpp"
#include "igneous/input/GamepadAxis.hpp"
#include "igneous/input/InputMapLoader.hpp"
#include "igneous/rendering/Window.hpp"
#include "igneous/resources/AudioStream.hpp"
#include "igneous/resources/ResourceManager.hpp"
#include "igneous/resources/Sprite.hpp"

#include "imgui.h"
#include "SDL3/SDL_render.h"

#include <algorithm>
#include <memory>
#include <random>
#include <string>
#include <vector>

namespace
{
    constexpr const char* kInputMapPath = "assets/input_map.json";

    constexpr const char* kPaddleLeftUp = "paddle_left_up";
    constexpr const char* kPaddleLeftDown = "paddle_left_down";
    constexpr const char* kPaddleRightUp = "paddle_right_up";
    constexpr const char* kPaddleRightDown = "paddle_right_down";
    constexpr const char* kToggleMode = "toggle_mode";
    constexpr const char* kRestart = "restart";
    constexpr const char* kQuit = "quit";

    constexpr float kPaddleW = 12.0f;
    constexpr float kPaddleH = 72.0f;
    constexpr float kBallSize = 10.0f;
    constexpr float kPaddleSpeed = 320.0f;
    constexpr float kBallSpeed = 260.0f;
    constexpr float kAiSpeed = 240.0f;
    constexpr float kMargin = 24.0f;

    constexpr const char* kCourtSprite = "assets/sprites/court.png";
    constexpr const char* kPaddleLeftSprite = "assets/sprites/paddle_left.png";
    constexpr const char* kPaddleRightSprite = "assets/sprites/paddle_right.png";
    constexpr const char* kBallSprite = "assets/sprites/ball.png";
    constexpr const char* kPaddleHitSound = "assets/sounds/paddle_hit.wav";
    constexpr const char* kWallBounceSound = "assets/sounds/wall_bounce.wav";
    constexpr const char* kScoreSound = "assets/sounds/score.wav";

    float Clamp(float value, float minValue, float maxValue)
    {
        return std::max(minValue, std::min(value, maxValue));
    }

    enum class PaddleControl
    {
        Cpu,
        KeyboardLeft,
        KeyboardRight,
        Gamepad,
    };

    struct PaddleAssignment
    {
        PaddleControl control = PaddleControl::Cpu;
        SDL_JoystickID gamepadId = 0;
        std::string label = "CPU";
    };

    float GamepadPaddleDelta(SDL_JoystickID gamepadId, float dt)
    {
        const float up = Engine::Input::GetSemanticGamepadAxis(gamepadId, Engine::GamepadAxis::LeftYUp);
        const float down = Engine::Input::GetSemanticGamepadAxis(gamepadId, Engine::GamepadAxis::LeftYDown);
        return (down - up) * kPaddleSpeed * dt;
    }
}

class PongScene : public Engine::Scene
{
  public:
    float leftPaddleY = 0.0f;
    float rightPaddleY = 0.0f;
    Engine::Vec2<float> ballPos{0.0f, 0.0f};
    Engine::Vec2<float> ballVisualPos{0.0f, 0.0f};
    Engine::Vec2<float> leftPaddlePos{0.0f, 0.0f};
    Engine::Vec2<float> rightPaddlePos{0.0f, 0.0f};
    Engine::Vec2<float> courtPos{0.0f, 0.0f};
    Engine::Vec2<float> ballVel{kBallSpeed, kBallSpeed * 0.65f};
    int leftScore = 0;
    int rightScore = 0;
    bool twoPlayer = false;
    bool forceVsCpu = false;
    size_t connectedGamepadCount = 0;
    PaddleAssignment leftAssignment{};
    PaddleAssignment rightAssignment{};
    std::mt19937 rng{std::random_device{}()};

    std::unique_ptr<Engine::Sprite> courtSprite;
    std::unique_ptr<Engine::Sprite> leftPaddleSprite;
    std::unique_ptr<Engine::Sprite> rightPaddleSprite;
    std::unique_ptr<Engine::Sprite> ballSprite;

    std::shared_ptr<Engine::AudioStream> paddleHitSound;
    std::shared_ptr<Engine::AudioStream> wallBounceSound;
    std::shared_ptr<Engine::AudioStream> scoreSound;

    void ReloadInputMap()
    {
        Engine::Input::SetInputMap(Engine::InputMapLoader::LoadFromFile(kInputMapPath));
    }

    void RefreshDeviceLayout()
    {
        const std::vector<SDL_JoystickID> gamepads = Engine::Input::GetConnectedGamepads();
        connectedGamepadCount = gamepads.size();

        if (gamepads.size() >= 2)
        {
            twoPlayer = true;
            leftAssignment = {PaddleControl::Gamepad, gamepads[0], "Gamepad 1 (left stick)"};
            rightAssignment = {PaddleControl::Gamepad, gamepads[1], "Gamepad 2 (left stick)"};
            return;
        }

        if (gamepads.size() == 1)
        {
            twoPlayer = true;
            leftAssignment = {PaddleControl::KeyboardLeft, 0, "Keyboard (W/S)"};
            rightAssignment = {PaddleControl::Gamepad, gamepads[0], "Gamepad (left stick)"};
            return;
        }

        leftAssignment = {PaddleControl::KeyboardLeft, 0, "Keyboard (W/S)"};
        if (forceVsCpu)
        {
            twoPlayer = false;
            rightAssignment = {PaddleControl::Cpu, 0, "CPU"};
        }
        else
        {
            twoPlayer = true;
            rightAssignment = {PaddleControl::KeyboardRight, 0, "Keyboard (arrows)"};
        }
    }

    void ApplyKeyboardPaddle(float& paddleY, bool useLeftKeys, float dt)
    {
        if (useLeftKeys)
        {
            if (Engine::Input::IsActionPressed(kPaddleLeftUp))
                paddleY -= kPaddleSpeed * dt;
            if (Engine::Input::IsActionPressed(kPaddleLeftDown))
                paddleY += kPaddleSpeed * dt;
        }
        else
        {
            if (Engine::Input::IsActionPressed(kPaddleRightUp))
                paddleY -= kPaddleSpeed * dt;
            if (Engine::Input::IsActionPressed(kPaddleRightDown))
                paddleY += kPaddleSpeed * dt;
        }
    }

    void ApplyPaddleControl(float& paddleY, const PaddleAssignment& assignment, float dt)
    {
        switch (assignment.control)
        {
            case PaddleControl::KeyboardLeft:
                ApplyKeyboardPaddle(paddleY, true, dt);
                break;
            case PaddleControl::KeyboardRight:
                ApplyKeyboardPaddle(paddleY, false, dt);
                break;
            case PaddleControl::Gamepad:
                paddleY += GamepadPaddleDelta(assignment.gamepadId, dt);
                break;
            case PaddleControl::Cpu:
            default:
                break;
        }
    }

    void SyncSpritePositions()
    {
        const float viewW = static_cast<float>(Engine::Window::viewport.x);
        const float viewH = static_cast<float>(Engine::Window::viewport.y);

        courtPos = {viewW * 0.5f, viewH * 0.5f};

        leftPaddlePos = {kMargin + kPaddleW * 0.5f, leftPaddleY + kPaddleH * 0.5f};
        rightPaddlePos = {viewW - kMargin - kPaddleW * 0.5f, rightPaddleY + kPaddleH * 0.5f};
        ballVisualPos = {ballPos.x + kBallSize * 0.5f, ballPos.y + kBallSize * 0.5f};

        if (courtSprite)
            courtSprite->position = courtPos;
        if (leftPaddleSprite)
            leftPaddleSprite->position = leftPaddlePos;
        if (rightPaddleSprite)
            rightPaddleSprite->position = rightPaddlePos;
        if (ballSprite)
            ballSprite->position = ballVisualPos;
    }

    void ResetBall(int towardLeftPlayer)
    {
        ballPos = {
            Engine::Window::viewport.x * 0.5f - kBallSize * 0.5f,
            Engine::Window::viewport.y * 0.5f - kBallSize * 0.5f,
        };

        std::uniform_int_distribution<int> coin(0, 1);
        const float vertical = coin(rng) == 0 ? 1.0f : -1.0f;
        const float direction = towardLeftPlayer < 0 ? -1.0f : 1.0f;
        ballVel = {kBallSpeed * direction, kBallSpeed * 0.55f * vertical};
    }

    void OnCreated() override
    {
        ReloadInputMap();
        RefreshDeviceLayout();

        leftPaddleY = Engine::Window::viewport.y * 0.5f - kPaddleH * 0.5f;
        rightPaddleY = leftPaddleY;
        ResetBall(1);

        courtSprite = std::make_unique<Engine::Sprite>(courtPos, kCourtSprite);
        courtSprite->centered = true;
        courtSprite->SetZIndex(-10);

        leftPaddleSprite = std::make_unique<Engine::Sprite>(leftPaddlePos, kPaddleLeftSprite);
        leftPaddleSprite->centered = true;

        rightPaddleSprite = std::make_unique<Engine::Sprite>(rightPaddlePos, kPaddleRightSprite);
        rightPaddleSprite->centered = true;

        ballSprite = std::make_unique<Engine::Sprite>(ballVisualPos, kBallSprite);
        ballSprite->centered = true;

        paddleHitSound = Engine::ResourceManager::LoadSound(kPaddleHitSound, 0);
        wallBounceSound = Engine::ResourceManager::LoadSound(kWallBounceSound, 0);
        scoreSound = Engine::ResourceManager::LoadSound(kScoreSound, 0);
    }

    void Update(double delta) override
    {
        const float dt = static_cast<float>(delta);
        const float viewW = static_cast<float>(Engine::Window::viewport.x);
        const float viewH = static_cast<float>(Engine::Window::viewport.y);

        const size_t gamepadCount = Engine::Input::GetConnectedGamepads().size();
        if (gamepadCount != connectedGamepadCount)
            RefreshDeviceLayout();

        leftPaddleY = Clamp(leftPaddleY, kMargin, viewH - kMargin - kPaddleH);
        rightPaddleY = Clamp(rightPaddleY, kMargin, viewH - kMargin - kPaddleH);

        ApplyPaddleControl(leftPaddleY, leftAssignment, dt);

        if (rightAssignment.control == PaddleControl::Cpu)
        {
            const float aiTarget = ballPos.y;
            if (rightPaddleY + kPaddleH * 0.5f < aiTarget)
                rightPaddleY += kAiSpeed * dt;
            else if (rightPaddleY + kPaddleH * 0.5f > aiTarget)
                rightPaddleY -= kAiSpeed * dt;
        }
        else
        {
            ApplyPaddleControl(rightPaddleY, rightAssignment, dt);
        }

        ballPos.x += ballVel.x * dt;
        ballPos.y += ballVel.y * dt;

        if (ballPos.y <= kMargin)
        {
            ballPos.y = kMargin;
            if (ballVel.y < 0.0f)
            {
                ballVel.y = -ballVel.y;
                if (wallBounceSound)
                    wallBounceSound->Play(0.6f);
            }
        }
        else if (ballPos.y + kBallSize >= viewH - kMargin)
        {
            ballPos.y = viewH - kMargin - kBallSize;
            if (ballVel.y > 0.0f)
            {
                ballVel.y = -ballVel.y;
                if (wallBounceSound)
                    wallBounceSound->Play(0.6f);
            }
        }

        const float leftX = kMargin;
        const float rightX = viewW - kMargin - kPaddleW;

        SDL_FRect leftPaddle{leftX, leftPaddleY, kPaddleW, kPaddleH};
        SDL_FRect rightPaddle{rightX, rightPaddleY, kPaddleW, kPaddleH};
        SDL_FRect ball{ballPos.x, ballPos.y, kBallSize, kBallSize};

        if (SDL_HasRectIntersectionFloat(&ball, &leftPaddle) && ballVel.x < 0.0f)
        {
            ballVel.x = std::abs(ballVel.x);
            ballVel.y += (ballPos.y + kBallSize * 0.5f - (leftPaddleY + kPaddleH * 0.5f)) * 2.5f;
            if (paddleHitSound)
                paddleHitSound->Play(0.85f);
        }

        if (SDL_HasRectIntersectionFloat(&ball, &rightPaddle) && ballVel.x > 0.0f)
        {
            ballVel.x = -std::abs(ballVel.x);
            ballVel.y += (ballPos.y + kBallSize * 0.5f - (rightPaddleY + kPaddleH * 0.5f)) * 2.5f;
            if (paddleHitSound)
                paddleHitSound->Play(0.85f);
        }

        const float maxVertical = kBallSpeed * 1.35f;
        ballVel.y = Clamp(ballVel.y, -maxVertical, maxVertical);

        if (ballPos.x + kBallSize < 0.0f)
        {
            ++rightScore;
            if (scoreSound)
                scoreSound->Play(1.0f);
            ResetBall(-1);
        }
        else if (ballPos.x > viewW)
        {
            ++leftScore;
            if (scoreSound)
                scoreSound->Play(1.0f);
            ResetBall(1);
        }
    }

    void Render() override
    {
        SyncSpritePositions();

        ImGui::Begin("Pong");
        ImGui::Text("Arcade Pong using Sprite + AudioStream.");
        ImGui::Separator();
        ImGui::Text("Engine features:");
        ImGui::BulletText("Sprite — court, paddles, and ball via ResourceManager");
        ImGui::BulletText("ResourceManager::LoadSound — hit, bounce, and score cues");
        ImGui::BulletText("Scene::Update — frame-rate independent motion (Time::deltaTime)");
        ImGui::BulletText("InputMapLoader — keyboard bindings in assets/input_map.json");
        ImGui::BulletText("Per-device paddles — gamepads assigned by connection order");
        ImGui::Separator();
        ImGui::Text("Input map: %s", kInputMapPath);
        ImGui::Text("Connected gamepads: %zu", connectedGamepadCount);
        ImGui::Text("Left paddle: %s", leftAssignment.label.c_str());
        ImGui::Text("Right paddle: %s", rightAssignment.label.c_str());
        if (ImGui::Button("Reload input map"))
            ReloadInputMap();
        ImGui::SameLine();
        ImGui::TextDisabled("(edit JSON while running, then reload)");
        ImGui::Separator();
        ImGui::Text("Score: %d  —  %d", leftScore, rightScore);
        ImGui::Text("Mode: %s", twoPlayer ? "2-player" : "vs CPU");
        if (connectedGamepadCount == 0)
            ImGui::Text("Tab/Start: toggle vs CPU / keyboard 2P (W/S vs arrows)");
        else
            ImGui::Text("Device split active (Tab only applies with no gamepads)");
        ImGui::Text("Delta: %.4f s", Engine::Time::deltaTime);
        ImGui::Separator();
        ImGui::Text("R: reset   Escape/Back: quit");
        ImGui::End();
    }

    void HandleInputs(Engine::InputLayer& layer) override
    {
        (void) layer;

        if (Engine::Input::IsActionJustPressed(kQuit))
            Engine::Engine::Quit();

        if (connectedGamepadCount == 0 && Engine::Input::IsActionJustPressed(kToggleMode))
        {
            forceVsCpu = !forceVsCpu;
            RefreshDeviceLayout();
        }

        if (Engine::Input::IsActionJustPressed(kRestart))
        {
            leftScore = 0;
            rightScore = 0;
            ResetBall(1);
        }
    }
};

int main()
{
    Engine::ResourceManager::SetScaleMode(SDL_SCALEMODE_NEAREST);

    Engine::EngineInitSettings settings;
    settings.width = 800;
    settings.height = 480;
    settings.title = "Pong (Igneous)";

    Engine::Engine engine;
    return engine.Run<PongScene>("Pong", settings);
}
