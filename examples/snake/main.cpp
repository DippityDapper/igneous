#include "igneous/engine/Engine.hpp"
#include "igneous/engine/Time.hpp"
#include "igneous/engine/Vec2.hpp"
#include "igneous/input/Input.hpp"
#include "igneous/input/InputMapLoader.hpp"
#include "igneous/rendering/Window.hpp"
#include "igneous/resources/AudioStream.hpp"
#include "igneous/resources/ResourceManager.hpp"
#include "igneous/resources/Sprite.hpp"

#include "imgui.h"

#include <algorithm>
#include <cmath>
#include <deque>
#include <memory>
#include <random>
#include <string>
#include <vector>

namespace
{
    constexpr const char* kInputMapPath = "assets/input_map.json";

    constexpr const char* kTurnUp = "turn_up";
    constexpr const char* kTurnDown = "turn_down";
    constexpr const char* kTurnLeft = "turn_left";
    constexpr const char* kTurnRight = "turn_right";
    constexpr const char* kSteerUp = "steer_up";
    constexpr const char* kSteerDown = "steer_down";
    constexpr const char* kSteerLeft = "steer_left";
    constexpr const char* kSteerRight = "steer_right";
    constexpr const char* kRestart = "restart";
    constexpr const char* kQuit = "quit";

    constexpr int kGridCols = 24;
    constexpr int kGridRows = 18;
    constexpr double kMoveInterval = 0.12;
    constexpr float kTargetCellPx = 16.0f;

    constexpr const char* kBoardSprite = "assets/sprites/board.png";
    constexpr const char* kSnakeHeadSprite = "assets/sprites/snake_head.png";
    constexpr const char* kSnakeBodySprite = "assets/sprites/snake_body.png";
    constexpr const char* kFoodSprite = "assets/sprites/food.png";
    constexpr const char* kEatSound = "assets/sounds/eat.wav";
    constexpr const char* kGameOverSound = "assets/sounds/game_over.wav";

    struct GridPoint
    {
        int x = 0;
        int y = 0;

        bool operator==(const GridPoint& other) const
        {
            return x == other.x && y == other.y;
        }
    };

    struct BoardLayout
    {
        float originX = 0.0f;
        float originY = 0.0f;
        float cellW = kTargetCellPx;
        float cellH = kTargetCellPx;
        float cellScale = 1.0f;
        Engine::Vec2<float> boardCenter{0.0f, 0.0f};
    };

    struct SegmentVisual
    {
        Engine::Vec2<float> position;
        std::unique_ptr<Engine::Sprite> sprite;
    };

    BoardLayout ComputeBoardLayout()
    {
        BoardLayout layout;
        layout.cellW = kTargetCellPx;
        layout.cellH = kTargetCellPx;

        const float boardW = kGridCols * layout.cellW;
        const float boardH = kGridRows * layout.cellH;
        layout.originX = (Engine::Window::viewport.x - boardW) * 0.5f;
        layout.originY = (Engine::Window::viewport.y - boardH) * 0.5f;
        layout.boardCenter = {layout.originX + boardW * 0.5f, layout.originY + boardH * 0.5f};
        layout.cellScale = 1.0f;
        return layout;
    }

    Engine::Vec2<float> GridToScreen(const GridPoint& cell, const BoardLayout& layout)
    {
        return {
            layout.originX + cell.x * layout.cellW + layout.cellW * 0.5f,
            layout.originY + cell.y * layout.cellH + layout.cellH * 0.5f,
        };
    }

    GridPoint RandomOpenCell(const std::deque<GridPoint>& snake, std::mt19937& rng)
    {
        std::vector<GridPoint> openCells;
        openCells.reserve(kGridCols * kGridRows);

        for (int y = 0; y < kGridRows; ++y)
        {
            for (int x = 0; x < kGridCols; ++x)
            {
                GridPoint candidate{x, y};
                if (std::find(snake.begin(), snake.end(), candidate) == snake.end())
                    openCells.push_back(candidate);
            }
        }

        if (openCells.empty())
            return {0, 0};

        std::uniform_int_distribution<size_t> pick(0, openCells.size() - 1);
        return openCells[pick(rng)];
    }

    /// Degrees clockwise; assumes head art faces right at 0°.
    float HeadRotationDegrees(const GridPoint& dir)
    {
        if (dir.x > 0)
            return 0.0f;
        if (dir.x < 0)
            return 180.0f;
        if (dir.y < 0)
            return 270.0f;
        if (dir.y > 0)
            return 90.0f;
        return 0.0f;
    }
}

class SnakeScene : public Engine::Scene
{
  public:
    std::deque<GridPoint> snake{{12, 9}, {11, 9}, {10, 9}};
    GridPoint direction{1, 0};
    GridPoint queuedDirection{1, 0};
    GridPoint food{16, 9};
    double moveTimer = 0.0;
    int score = 0;
    bool gameOver = false;
    bool playedGameOverSound = false;
    GridPoint lastStickDirection{};
    std::mt19937 rng{std::random_device{}()};

    Engine::Vec2<float> foodPosition;
    Engine::Vec2<float> boardPosition;
    std::unique_ptr<Engine::Sprite> foodSprite;
    std::unique_ptr<Engine::Sprite> boardSprite;
    std::deque<SegmentVisual> segments;

    std::shared_ptr<Engine::AudioStream> eatSound;
    std::shared_ptr<Engine::AudioStream> gameOverSound;

    void ReloadInputMap()
    {
        Engine::Input::SetInputMap(Engine::InputMapLoader::LoadFromFile(kInputMapPath));
    }

    void QueueDirection(GridPoint next)
    {
        if (next.x == -direction.x && next.y == -direction.y)
            return;
        queuedDirection = next;
    }

    void PollStickTurn()
    {
        const float steerX = Engine::Input::GetAxis(kSteerRight, kSteerLeft);
        const float steerY = Engine::Input::GetAxis(kSteerDown, kSteerUp);

        GridPoint stick{0, 0};
        if (std::abs(steerX) > 0.5f || std::abs(steerY) > 0.5f)
        {
            if (std::abs(steerX) >= std::abs(steerY))
                stick = steerX > 0.0f ? GridPoint{1, 0} : GridPoint{-1, 0};
            else
                stick = steerY > 0.0f ? GridPoint{0, 1} : GridPoint{0, -1};
        }

        if (stick.x == 0 && stick.y == 0)
        {
            lastStickDirection = {0, 0};
            return;
        }

        if (stick.x == lastStickDirection.x && stick.y == lastStickDirection.y)
            return;

        lastStickDirection = stick;
        QueueDirection(stick);
    }

    void EnsureSegmentCount(size_t count)
    {
        while (segments.size() < count)
        {
            segments.push_back({});
            SegmentVisual& segment = segments.back();
            const bool isHead = segments.size() == 1;
            segment.sprite = std::make_unique<Engine::Sprite>(
                segment.position,
                isHead ? kSnakeHeadSprite : kSnakeBodySprite);
            segment.sprite->centered = true;
            segment.sprite->SetZIndex(isHead ? 2 : 1);
        }

        for (size_t i = 0; i < segments.size(); ++i)
            segments[i].sprite->render = i < count;
    }

    void SyncVisuals(const BoardLayout& layout)
    {
        EnsureSegmentCount(snake.size());

        for (size_t i = 0; i < snake.size(); ++i)
        {
            segments[i].position = GridToScreen(snake[i], layout);
            segments[i].sprite->scaleX = layout.cellScale;
            segments[i].sprite->scaleY = layout.cellScale;
            segments[i].sprite->rotation = i == 0 ? HeadRotationDegrees(queuedDirection) : 0.0f;
        }

        if (foodSprite)
        {
            foodPosition = GridToScreen(food, layout);
            foodSprite->scaleX = layout.cellScale;
            foodSprite->scaleY = layout.cellScale;
        }

        boardPosition = layout.boardCenter;

        if (boardSprite)
            boardSprite->position = boardPosition;
    }

    void ResetGame()
    {
        snake = {{12, 9}, {11, 9}, {10, 9}};
        direction = {1, 0};
        queuedDirection = {1, 0};
        food = RandomOpenCell(snake, rng);
        moveTimer = 0.0;
        score = 0;
        gameOver = false;
        playedGameOverSound = false;
        lastStickDirection = {0, 0};
    }

    void OnCreated() override
    {
        ReloadInputMap();

        foodSprite = std::make_unique<Engine::Sprite>(foodPosition, kFoodSprite);
        foodSprite->centered = true;
        foodSprite->SetZIndex(5);

        boardSprite = std::make_unique<Engine::Sprite>(boardPosition, kBoardSprite);
        boardSprite->centered = true;
        boardSprite->SetZIndex(-10);

        eatSound = Engine::ResourceManager::LoadSound(kEatSound, 0);
        gameOverSound = Engine::ResourceManager::LoadSound(kGameOverSound, 0);

        EnsureSegmentCount(snake.size());
    }

    void Update(double delta) override
    {
        if (gameOver)
            return;

        moveTimer += delta;
        if (moveTimer < kMoveInterval)
            return;

        moveTimer = 0.0;
        direction = queuedDirection;

        GridPoint head = snake.front();
        head.x += direction.x;
        head.y += direction.y;

        if (head.x < 0 || head.x >= kGridCols || head.y < 0 || head.y >= kGridRows)
        {
            gameOver = true;
            if (gameOverSound && !playedGameOverSound)
            {
                gameOverSound->Play(1.0f);
                playedGameOverSound = true;
            }
            return;
        }

        if (std::find(snake.begin(), snake.end(), head) != snake.end())
        {
            gameOver = true;
            if (gameOverSound && !playedGameOverSound)
            {
                gameOverSound->Play(1.0f);
                playedGameOverSound = true;
            }
            return;
        }

        snake.push_front(head);

        if (head == food)
        {
            ++score;
            if (eatSound)
                eatSound->Play(0.9f);
            food = RandomOpenCell(snake, rng);
        }
        else
        {
            snake.pop_back();
        }
    }

    void Render() override
    {
        const BoardLayout layout = ComputeBoardLayout();
        SyncVisuals(layout);

        ImGui::Begin("Snake");
        ImGui::Text("Grid Snake using Sprite + AudioStream.");
        ImGui::Separator();
        ImGui::Text("Engine features:");
        ImGui::BulletText("Sprite — head/body/food drawn via ResourceManager");
        ImGui::BulletText("Sprite::rotation — head turns with movement (degrees clockwise)");
        ImGui::BulletText("ResourceManager::LoadSound — eat and game-over cues");
        ImGui::BulletText("Scene::Update — fixed-step movement (Time::deltaTime)");
        ImGui::BulletText("InputMapLoader — bindings in assets/input_map.json");
        ImGui::Separator();
        ImGui::Text("Input map: %s", kInputMapPath);
        ImGui::Text(
            "Stick: (%.2f, %.2f)",
            Engine::Input::GetAxis(kSteerRight, kSteerLeft),
            Engine::Input::GetAxis(kSteerDown, kSteerUp));
        if (ImGui::Button("Reload input map"))
            ReloadInputMap();
        ImGui::SameLine();
        ImGui::TextDisabled("(edit JSON while running, then reload)");
        ImGui::Separator();
        ImGui::Text("Score: %d", score);
        ImGui::Text("Delta: %.4f s", Engine::Time::deltaTime);
        if (gameOver)
            ImGui::TextColored({1.0f, 0.45f, 0.45f, 1.0f}, "Game over — press R to restart");
        else
            ImGui::Text("Steer: WASD / arrows / d-pad / left stick   Restart: R   Quit: Escape");
        ImGui::End();
    }

    void HandleInputs(Engine::InputLayer& layer) override
    {
        (void) layer;

        if (Engine::Input::IsActionJustPressed(kQuit))
            Engine::Engine::Quit();

        if (gameOver)
        {
            if (Engine::Input::IsActionJustPressed(kRestart))
                ResetGame();
            return;
        }

        if (Engine::Input::IsActionJustPressed(kTurnUp))
            QueueDirection({0, -1});
        if (Engine::Input::IsActionJustPressed(kTurnDown))
            QueueDirection({0, 1});
        if (Engine::Input::IsActionJustPressed(kTurnLeft))
            QueueDirection({-1, 0});
        if (Engine::Input::IsActionJustPressed(kTurnRight))
            QueueDirection({1, 0});

        PollStickTurn();
    }
};

int main()
{
    Engine::ResourceManager::SetScaleMode(SDL_SCALEMODE_NEAREST);

    Engine::EngineInitSettings settings;
    settings.width = 640;
    settings.height = 480;
    settings.title = "Snake (Igneous)";

    Engine::Engine engine;
    return engine.Run<SnakeScene>("Snake", settings);
}
