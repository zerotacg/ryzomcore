add_test_executable(ryzom_game_share_test
        #prim_object_test.cpp
        test_module_interface.cpp
        test_module_itf.cpp
)

target_link_libraries(ryzom_game_share_test
        PRIVATE
        NeL::misc
        NeL::net
)
