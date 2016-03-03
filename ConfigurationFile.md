# The Config File #

| **Entry** | **Description** | **Default** |
|:----------|:----------------|:------------|
| cam\_height | height of the gamearea | 580         |
| cam\_width | width of the gamearea | 780         |
| log\_file | location of the last played log file |             |
| refbox\_ip | IP of the refbox | 224.5.23.1  |
| refbox\_port | Port of the refbox | 10001       |
| ssl\_vision\_ip | IP of the SSL Vision | 224.5.23.2  |
| ssl\_vision\_port | Port of the SSL Vision | 10002       |

**Location of the config file:**
```
~/.ssl-autonomous-refbox/ssl-autonomous-refbox.conf
```


# Using config #
**Add entry to config:**
```
Global::config.add("<entry>", <value>);
```

**Get entry from config:**
```
Global::config.readInto(<value>, "<entry>");
```

**Save config:**
```
Global::saveConfig();
```