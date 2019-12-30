# mygc
mygc is a precise, generation distinguished, stop the world garbage collection library for C++

## How to use:
    mygc::gc_root<int> p;
    p = mygc::make_gc<int>(1); // allocate objects in gc heap
    p = mygc::make_gc<int>(2); // allocate another
    auto p2 = mygc::make_gc<int>(3);
    p = p2; //lvalue assign is allowed
    p = nullptr;
  
    mygc::gc_root<int[]> array = mygc::make_gc<int[]>(3); //create an array of three int objects in gc heap;
    
    class Foo {
      gc_ptr<int> myInt; // a gc reference which is not a root
    }
    
as you can see, using `gc_ptr` is simlar to use `unique_ptr` except that you can only use `make_gc` to allocate objects. 

## What is the difference between gc_ptr and gc_root?
`gc_root` stands for a gc root reference. while `gc_ptr` is just a normal gc reference.

## What is a gc root reference?
The concept "gc root" has slightly differences between languages, but simply speaking, that is the reference which you can access directly, and other references which are not roots must have a root reference ancestor. If the life time of the root gc root reference is over, all it's children is considered unreachable, which can be collected safely.

## I still don't understand gc root.
Simply, use `gc_root` when you need to define a variable, use `gc_ptr` when you need to define a member. More simple, a `gc_ptr` sits on the *right* side of a member accessor (`->` or '.'), for other cases use `gc_root`.

## Why can't you hide the concept `root` like other languages do?
For performance. At first I did hide this concept, and let the library considered if a reference is a root on runtime, but this checking take times, especially when large objects generation is added. Iterate objects in large generation takes a lot of time. Another reason is that if I hide the root concept, a reference class must got a destructor, which makes all classes who have a gc reference as a member non-trivial-destructable. The collector will have to scan and pick the non-trivial-destructable objects even the destructor does nothing, this takes time as well.

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
