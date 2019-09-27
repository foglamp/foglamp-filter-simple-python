FogLAMP Simple Python filter plugin
===================================

The simple Python filter plugin is analogous to the expression filter but accept Python code rather than the expression syntax.

Differently from FogLAMP Python2 and Python3 filters plugins which can process all the incoming data and configuration by loading a Python script, this filter can only transform a single item of the incoming reading data and cannot access any configuration option.

.. note::
   Python3.x development package is required.
   
Configuration
-------------

This filter requires following configuration option:

code
  The Python code that will be applied to filter a reading data

The following examples show how to filter the readings data,

- Change datapoint value  

.. code-block:: console

    reading['point_1'] = reading['point_1'] * 2 + 15

- Create a new datapoint while filtering

.. code-block:: console

    reading[b'temp_fahr'] = reading[b'temperature'] * 9 / 5 + 32

- Generate an exponential moving average (ema)
   
   In this case we need to parse some data while filtering current dataset the filter receives in input. 
   A global 'user_data' empty dictionary is available to the Python interpreter and key values can be easily added.

.. code-block:: console

    global user_data
    if not user_data:
        user_data['latest'] = None
    for attribute in list(reading):
        if not user_data['latest']:
            user_data['latest'] = reading[attribute]
        user_data['latest'] = reading[attribute] * 0.07 + user_data['latest'] * (1 - 0.07)
        reading[b'ema'] = user_data['latest']


How to add Python code to the filter
------------------------------------

Python code can be entered using FogLAMP REST API call, example with curl:

.. code-block:: console

    reading['point_1'] = reading['point_1'] * 2 + 15

.. code-block:: console

    $ curl -X POST "http://127.0.0.1:8081/foglamp/filter" -d '
    {
        "name":"CodePy3",
        "plugin":"simplepython",
        "filter_config": {
            "code":"reading[b'\''point_1'\''] = reading[b'\''point_1'\''] * 2  + 15",
            "enable":"true"
        }
    }'

Note: The escape for single quote character in the payload.

It is also possible to send the required JSON payload via REST API call using a file, e.g. filter.json

.. code-block:: console

    $ curl -X POST -H "Content-Type: application/json" -d @filter.json http://127.0.0.1:8081/foglamp/filter

This way the escape for the single quote character is not needed.

Python code can also be entered using a textbox available (for code) in FogLAMP GUI (single quote character escape is not needed).

Build
-----

To build FogLAMP "simple-python" C++ filter plugin:

.. code-block:: console

  $ mkdir build
  $ cd build
  $ cmake ..

- By default the FogLAMP develop package header files and libraries
  are expected to be located in /usr/include/foglamp and /usr/lib/foglamp
- If **FOGLAMP_ROOT** env var is set and no -D options are set,
  the header files and libraries paths are pulled from the ones under the
  FOGLAMP_ROOT directory.
  Please note that you must first run 'make' in the FOGLAMP_ROOT directory.

You may also pass one or more of the following options to cmake to override 
this default behaviour:

- **FOGLAMP_SRC** sets the path of a FogLAMP source tree
- **FOGLAMP_INCLUDE** sets the path to FogLAMP header files
- **FOGLAMP_LIB sets** the path to FogLAMP libraries
- **FOGLAMP_INSTALL** sets the installation path of Random plugin

NOTE:
 - The **FOGLAMP_INCLUDE** option should point to a location where all the FogLAMP 
   header files have been installed in a single directory.
 - The **FOGLAMP_LIB** option should point to a location where all the FogLAMP
   libraries have been installed in a single directory.
 - 'make install' target is defined only when **FOGLAMP_INSTALL** is set

Examples:

- no options

  $ cmake ..

- no options and FOGLAMP_ROOT set

  $ export FOGLAMP_ROOT=/some_foglamp_setup

  $ cmake ..

- set FOGLAMP_SRC

  $ cmake -DFOGLAMP_SRC=/home/source/develop/FogLAMP  ..

- set FOGLAMP_INCLUDE

  $ cmake -DFOGLAMP_INCLUDE=/dev-package/include ..

- set FOGLAMP_LIB

  $ cmake -DFOGLAMP_LIB=/home/dev/package/lib ..

- set FOGLAMP_INSTALL

  $ cmake -DFOGLAMP_INSTALL=/home/source/develop/FogLAMP

  $ cmake -DFOGLAMP_INSTALL=/usr/local/foglamp
