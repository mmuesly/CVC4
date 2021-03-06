set(EXAMPLES_API_JAVA
  ## disabled until bindings for the new API are in place (issue #2284)
  # BitVectors
  # BitVectorsAndArrays
  # CVC4Streams
  # Combination
  # Datatypes
  # Exceptions
  # FloatingPointArith
  # HelloWorld
  # LinearArith
  # PipedInput
  # Relations
  # Statistics
  # Strings
  # UnsatCores
)

foreach(example ${EXAMPLES_API_JAVA})
  add_jar(${example} ${example}.java
          INCLUDE_JARS "${CVC4_JAR}"
          OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin/api/java")

  set(EXAMPLE_TEST_NAME api/java/${example})

  add_test(
    NAME ${EXAMPLE_TEST_NAME}
    COMMAND
      ${Java_JAVA_EXECUTABLE}
        -cp "${CVC4_JAR}:${CMAKE_BINARY_DIR}/bin/api/java/${example}.jar"
        -Djava.library.path=${CVC4_JNI_PATH}
        ${example}
  )
  set_tests_properties(${EXAMPLE_TEST_NAME} PROPERTIES SKIP_RETURN_CODE 77)
endforeach()
