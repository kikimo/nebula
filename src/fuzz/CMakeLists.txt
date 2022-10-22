set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,fuzzer -g")
# add_executable(graph_fuzz graph_fuzz.cpp)

nebula_add_executable(
    NAME
        hello-fuzzer
    SOURCES
        HelloFuzzer.cpp 
)

nebula_add_executable(
    NAME
        graph-fuzzer
    SOURCES
        GraphFuzzer.cpp 
    OBJECTS
        $<TARGET_OBJECTS:graph_stats_obj>
        $<TARGET_OBJECTS:meta_client_stats_obj>
        $<TARGET_OBJECTS:storage_client_stats_obj>
        $<TARGET_OBJECTS:util_obj>
        $<TARGET_OBJECTS:service_obj>
        $<TARGET_OBJECTS:graph_session_obj>
        $<TARGET_OBJECTS:query_engine_obj>
        $<TARGET_OBJECTS:parser_obj>
        $<TARGET_OBJECTS:ast_match_path_obj>
        $<TARGET_OBJECTS:validator_obj>
        $<TARGET_OBJECTS:expr_visitor_obj>
        $<TARGET_OBJECTS:optimizer_obj>
        $<TARGET_OBJECTS:plan_node_visitor_obj>
        $<TARGET_OBJECTS:planner_obj>
        $<TARGET_OBJECTS:plan_obj>
        $<TARGET_OBJECTS:executor_obj>
        $<TARGET_OBJECTS:scheduler_obj>
        $<TARGET_OBJECTS:idgenerator_obj>
        $<TARGET_OBJECTS:graph_context_obj>
        $<TARGET_OBJECTS:graph_flags_obj>
        $<TARGET_OBJECTS:graph_auth_obj>
        $<TARGET_OBJECTS:graph_thrift_obj>
        $<TARGET_OBJECTS:storage_client_base_obj>
        $<TARGET_OBJECTS:storage_client_obj>
        $<TARGET_OBJECTS:charset_obj>
        $<TARGET_OBJECTS:graph_obj>
        $<TARGET_OBJECTS:ft_es_graph_adapter_obj>
        $<TARGET_OBJECTS:storage_thrift_obj>
        $<TARGET_OBJECTS:gc_obj>
        ${common_deps}
    LIBRARIES
        ${PROXYGEN_LIBRARIES}
        ${THRIFT_LIBRARIES}
        wangle
)

