# member_thunk

## Custom heap

To implement features such as W^X and Control Flow Guard, member_thunk implements its own heap. There is a built-in default heap for user convenience, but it can be disabled by defining `MEMBER_THUNK_DISABLE_DEFAULT_HEAP`. The API is fairly simple:

 - Use `allocate_page()` to get a memory page suitable for creating thunks.
 - Use `compact()` to compact the heap (release unused memory regions).

If you desire managing member_thunk's heaps yourself, use the `heap` class. The API is for this class is the same as above, except the functions are class members.

## Thread safety

member_thunk implements thread safety through the use of template parameters (the default template parameter varies according to `MEMBER_THUNK_DEFAULT_HEAP_SAFETY`, see table below):

 - `heap<null_lock>` does not have thread safety;
 - `heap<slim_lock>` provides thread safety for compacting the heap as well as creating and freeing a page.

Despite `page` taking an (also defaulted) lock template parameter, it does not provide safety for operations within a page: assume it to be single-threaded or implement your own locking on top. The template parameter is used because a page keeps a pointer to the heap it comes from, so also needs to know its locking strategy.

Concurrent execution of methods in a single page works lock-free no matter what option is picked, however all execution must be stopped before destroying the page.

`slim_lock` is a custom type which uses the fast and efficient Windows [Slim Reader/Writer Locks](https://docs.microsoft.com/en-us/windows/win32/sync/slim-reader-writer--srw--locks). If you prefer, you can use any type that meets the [BasicLockable](https://en.cppreference.com/w/cpp/named_req/BasicLockable) requirement and is default constructible, for example `std::mutex`.

The default heap has 3 different ways of operating with regards to thread safety:

| Type                 | Value of `MEMBER_THUNK_DEFAULT_HEAP_SAFETY` | Description                                                                           | Default template parameter |
| :------------------- | :------------------------------------------ | :------------------------------------------------------------------------------------ | :------------------------- |
| Thread-safe heap     | 3                                           | There is only one heap, and it is thread-safe. This is the default.                   | `slim_lock`                |
| Thread-local heap    | 2                                           | There is one heap per thread, and there is no locking happening within a single heap. | `null_lock`                |
| Single-threaded heap | 1                                           | There is only one heap, and it is not thread-safe.                                    | `null_lock`                |

> **Note:** If the default heap is disabled as documented in [custom heap](#custom-heap), then the default template parameter is always `null_lock` as it is assumed you handle thread safety yourself.
