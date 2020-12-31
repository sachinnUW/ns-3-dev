# System import
import os

# Perform configuration and build
print("Configure NS-3.")
os.system('./waf configure --enable-examples --enable-tests --enable-sudo --disable-python')
print("Build NS-3.")
os.system('./waf')

# Set up environment variables.
print("Setting up NS-3 environment.")
os.system('./waf shell')



