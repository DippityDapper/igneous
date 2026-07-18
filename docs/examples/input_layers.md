# input_layers

**Target:** `example_input_layers`  
**Feature:** Input layers, priority dispatch, and consumption

Registers `gameplay`, `ui`, and `menu` layers. Tab opens a menu; the menu layer calls `HandleAllInputs` so gameplay does not receive WASD until the menu closes. Escape closes the menu or quits when the menu is closed.

## Assets

None.

## Controls

- **WASD** — move player (gameplay layer, when menu closed)
- **Tab** — toggle menu
- **Escape** — close menu, or quit when menu is closed

## Related

- [Input](../classes/Input.md)
- [InputLayer](../classes/InputLayer.md)
- [InputEvent](../classes/InputEvent.md)
