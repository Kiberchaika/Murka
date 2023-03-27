# Murka
![](https://i.imgur.com/zJLcsau.png)

## Introduction
A feline inspired tiny mixed-mode UI library for C++. It allows you to build fairly complex UIs in both object-oriented and immediate mode ways. Biggest upsides is that it's easy to write new widgets with it with custom look and feel. Still in fairly heavy development, expect API changes. I plan to release a version 1.0 somewhere around September.

[openFrameworks addon](https://github.com/Kiberchaika/ofxMurka)

## More info and demo soon!

### Some notes on next API changes
- Copying a context is a source for bad bugs and should never be attempted. That's because the context inside widget draw function is a link to the currentContext in the main Murka object. Copying it deactivates it. I'll remove this note when I'll add a prohibition for context copying or will otherwise change the pattern.
- `draw()` virtual function in View class should be renamed to something more specific soon because it clashes sometimes with other similarly called functions when you inherit View in your program, and there's no reason for it to have such a nice short name cause you're not supposed to call it yourself, it's for the library itself.
- `dataToControl` variable will become statically typed for you to never mistake what kind of data is expected to be controlled by the widget.

### Some notes on architecture

- There are 2 instances on internal > external class relationships in the library. MurkaViewInterface (external) and View (internal), and the MurkaViewHandler (external) and MurkaViewHandlerInternal (internal). The naming is not entirely consistent yet, that's why I'm writing it down here and this text will stay until I improve the naming.
- What are those 2 sets of classes for? Murka provides an extremely easy to use interface for immediate mode custom widget programming. Each widget has optional data that it controls (that's a pointer to some data that it knows what to do with), parameters that are defined in that widget, and results that are also defined in the widget code. You don't need to define anything else though, Murka does everything else for you. Hence these 2 external/internal entities.
- MurkaViewInterface is a templated class that is used to allocate the widget object and results defined in your custom widgets. You don't need to do any of that yourself, you just define a results in your View code and call it Results, Murka allocates them through MurkaViewInterface. This Interface also has some wrapper functions. It's a child of non-templated View class that has some functions that basically allow any view to work. Hence, custom widgets inherit the MurkaViewInterface class.
- I will probably merge MurkaViewInterface and View into a single templated class, thus killing the anonymous View. Maybe.
- MurkaViewHandler is a templated class that wraps the View object with everything it needs to get from external code that calls the drawing - Parameters object, Results object, Data to control and the object itself. The internal part, MurkaViewHandlerInternal, is not templated and it just holds anonymous pointers.
- MurkaContext is global in weird way currently, it's a variable of Murka class that gets propagated through the graph of widgets via pushContext and popContext that's forwarded through lambda from the "singleton" main class. This is not a good pattern and the downside is, you should never ever try to pass around MurkaContext not by lvalue (&), if you do that it'll break things in weird places. At the very least, something won't render.

## Compiler Options

- `MURKA_DEBUG` - displays cout style debug messages