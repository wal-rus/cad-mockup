set(CadQuote_SOURCES
  JsonSerialization.cpp
  JsonSerialization.h
  MachineInfo.h
  MachineInfo.cpp
  picojson.h
  ToolPath.cpp
  ToolPath.h
  main.cpp
  Vector2.h
)

add_executable(CadQuote ${CadQuote_SOURCES})

