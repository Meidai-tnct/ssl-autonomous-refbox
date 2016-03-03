# Guidance #

Documentation of source code is built with doxygen. It provides Java-doc like tags, that will be used to parse everything to a HTML based documentation.
Some things should be considered:

  * each block should have a @brief
  * methods and functions should have @param and @return tags with description, if neccassary
  * @class should be above class definition in header file
  * @file should be on top of each file
  * documentation blocks should be primary in source files, not in headers