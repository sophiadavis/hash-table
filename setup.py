from distutils.core import setup, Extension
setup(name="hashtable", version="1.0",
      ext_modules=[
         Extension("hashtable", ["hashtablemodule_helpers.c",
                                 "hashtablemodule.c",
                                 "hashtable.c"])])
