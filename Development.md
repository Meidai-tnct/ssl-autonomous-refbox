# Commit #

Before you commit something, here are some rules:
  * There are no compile errors
  * You have formatted the code (see below)
  * You have no compiled or generated files in your commit
    * To make sure, everything is ok, run `./before_commit.sh`

# Code formatter #

There is a config file `astyle.conf` that is used by astyle to format and indent all .cc and .cpp files.

To install astyle, simply install the package
```
sudo apt-get install astyle
```

To format all source files, simply type
```
./format.sh
```

# log levels #
There are different log levels, that can be used (ordered by priority):
  * OFF = no logs
  * ERROR = errors that WILL result cause problems to the runtime
  * WARN = something happened, that should not be, but program will run stable
  * INFO = useful information during runtime, such as important variable values, etc.
  * DEBUG = debugging information, that is not interesting during productive runtime, but only for development
  * TRACE = really unimportant, but still useful at a certain point

Please always check to have the right log level for your purpose!