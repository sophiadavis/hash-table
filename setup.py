from distutils.core import setup, Extension
setup(name="hashtable", version="1.0",
      ext_modules=[
         Extension("hashtable", ["hashtablemodule.c", "hashtable.c"])])
