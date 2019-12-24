#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)
COMPONENT_EMBED_TXTFILES := server_root_cert.pem
CFLAGS+= -DLV_CONF_INCLUDE_SIMPLE
COMPONENT_EMBED_FILES := favicon.ico
COMPONENT_EMBED_FILES += upload_script.html
