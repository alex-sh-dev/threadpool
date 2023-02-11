add_executable(threadpoolDemo
    main.cpp
    utils.h
    tasks.h
    tasksholder.h
    common.h
)

target_link_libraries(threadpoolDemo threadpool)
