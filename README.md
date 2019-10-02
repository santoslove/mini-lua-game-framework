# A mini Lua game framework

This is:
* a simple example of SDL in use
* a simple example of Lua's C API in use
* a simple example of a game framework

Unfortunately, it's written badly and I don't really know how to use the Lua and SDL APIs or how to program in C, and it probably has lots of bugs.

But, it still might be fun for someone out there to read and experiment with.

## Compilation instructions for Windows

1. Install MinGW
2. Unzip engine.zip
3. Run compile.bat

## The API

### Callbacks

```lua
engine.frame() -- Called 60 times per second
engine.down(string) -- When a key or mouse button is pressed
engine.up(string) -- When a key or mouse button is released
```

### Window settings

```lua
engine.width = number
engine.height = number
engine.scale = number -- Window and graphics scale up by this number
```

### Input

```lua
boolean = engine.isDown(string) -- True if the given key or mouse button is down
number = engine.mouseX()
number = engine.mouseY()
```

### Graphics

```lua
engine.image(path, x, y, angle, scaleX, scaleY, originX, originY, r, g, b, a)
engine.line(x1, y1, x2, y2, r, g, b, a)
engine.rectangle(x, y, width, height, r, g, b, a)
engine.text(message, fontPath, fontSize, x, y, r, g, b, a)
```

### Sound

```lua
engine.sound(path)
engine.loop(path)
engine.stopLoop()
```

## Example

```lua
engine.width = 400
engine.height = 300
engine.scale = 2

player = {x = 0, y = 0}

function engine.frame()
    if engine.isDown('right') or engine.isDown('d') then
        player.x = player.x + 1
    end
    
    if engine.isDown('mouseLeft') then
        player.y = player.y + 1
    end
    
    engine.image('player.png', player.x, player.y)
end

function engine.down(input)
    if input == 'space' then
        player.x = 0
    end
end
```
