# Murka
![](https://i.imgur.com/zJLcsau.png)

## Introduction
A feline inspired tiny mixed-mode UI library for C++. It allows you to build fairly complex UIs in both object-oriented and immediate mode ways. Currently in heavy development (summer 2019).

[openFrameworks addon](https://github.com/Kiberchaika/ofxMurka)

## More info and demo soon!

### Some notes on architecture

- There are 2 instances on internal > external class relationships in the library. MurkaViewInterface (external) and View (internal), and the MurkaViewHandler (external) and MurkaViewHandlerInternal (internal). The naming is not consistent yet, that's why I'm writing it down here and this text will stay until I fix the naming.
- What are those 2 sets of classes for? Murka provides an extremely easy to use interface for immediate mode custom widget programming. Each widget has optional data that it controls (that's a pointer to some data that it knows what to do with), parameters that are defined in that widget, and results that are also defined in the widget code. You don't need to define anything else though, Murka does everything else for you. Hence these 2 external/internal entities.
- MurkaViewInterface is a templated class that is used to allocate the widget object and results defined in your custom widgets. You don't need to do any of that, you just define results in your code and call it Results, Murka allocates them through MurkaViewInterface. It also has some wrapper functions. It's a child of non-templated View class that has some functions that basically allow any view to work. Hence, custom widgets inherit the MurkaViewInterface class.
- I will probably merge MurkaViewInterface and View into a single templated class, thus killing the anonymous View.
- MurkaViewHandler is a templated class that wraps the View object with everything it needs to get from external code that calls the drawing - Parameters object, Results object, Data to control and the object itself. The internal part, MurkaViewHandlerInternal, is not templated and it just holds anonymous pointers.
- MurkaContext is global in weird way currently, it's a variable of Murka class that gets propagated through the graph of widgets via pushContext and popContext that's forwarded through lambda from the "singleton" main class. This is not a good pattern and the downside is, you should never ever try to pass around MurkaContext not by lvalue (&), that might break things. At the very least, something won't render. I think I'll probably change how it works at some point, for now it works though. Just be careful to only pass it by lvalue.
