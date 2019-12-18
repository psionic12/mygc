# mygc
mygc is a precise, generation distinguished, stop the world garbage collection library for C++

## How to use:
    mygc::gc_ptr<int> p;
    p = mygc::make_gc<int>(1); // allocate objects in gc heap
    p = mygc::make_gc<int>(2); // allocate another
    auto p2 = mygc::make_gc<int>(3);
    p = p2; //lvalue assign is allowed
    p = nullptr;
  
    mygc::gc_ptr<int[]> array = mygc::make_gc<int[]>(3); //create an array of three int objects in gc heap;
    
as you can see, using `gc_ptr` is simlar to use `unique_ptr` except that you can only use `make_gc` to allocate objects. 

## When will the destructor get called?
When an object is about to get collected, that means when the destructor will get called is un-predictable.

## How can a collector in C++ be precise?
By calcuating the references in runtime.
The calculation sits in `make_gc`, that why mygc only allows to use `make_gc` to allocate objects.
This calculation only be done once per Type.

## How the stop-the-world is implement?
By using signals(supported by `pthread` library), that's why you platform should support `pthread`.


## Configurations

### 1. MYGC_STOP_SIGNAL
define this to change the default signal number mygc used to stop the world. 

### 2. MYGC_TOTAL_SIZE
define the maximum size gc can use.

## Can I use this in other languages?
Yes. mygc is seprated by two parts: the core gc library and a wrapper header for C++, but you have to write you own wrapper.

## TODO

1. Illustrate the details of the core gc library and the C++ wrapper
2. Find a dark magic(if exist) to make the `gc_ptr` has destructor but make the `std::is_trivially_destructible<gc_ptr>` return true.
3. Or distinguish gc roots with other normal references.
