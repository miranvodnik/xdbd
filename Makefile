
# SUBDIRS=xdbd xdbd-test dbadt-test xdbd-gen xdbd-sql-parser xdbd-msg xdbd-trigger xdbd-api xdbd-odbc xdbd-nmgen-lib xdbd-nmgen xdbd-sysgen xdbd-nmgen-gen xdbd-mpx
SUBDIRS=xdbd xdbd-core xdbd-test xdbd-sql-parser xdbd-msg xdbd-mpx xdbd-api xdbd-odbc xdbd-nmgen-lib xdbd-nmgen xdbd-sysgen xdbd-errinfo xdbd-ham xdbd-stat

.PHONY: subdirs $(SUBDIRS)

all clean: subdirs

subdirs: $(SUBDIRS)

$(SUBDIRS):
	cd $@/; $(MAKE) TEST=$(TEST) CONFIGURATION=$(CONFIGURATION) $(MAKECMDGOALS)

xdbd: xdbd-core xdbd-sql-parser xdbd-trigger xdbd-stat

xdbd-core: xdbd-odbc xdbd-sql-parser

xdbd xdbd-test dbadt-test: xdbd-odbc

xdbd-odbc: xdbd-api

xdbd-sql-parser xdbd-api xdbd-ham: xdbd-msg xdbd-mpx xdbd-stat

xdbd-nmgen xdbd-sysgen: xdbd-nmgen-lib

xdbd-nmgen-mq xdbd-nmgen-sdl: xdbd-nmgen

xdbd xdbd-test xdbd-sql-parser xdbd-msg xdbd-mpx xdbd-api xdbd-odbc xdbd-nmgen-lib xdbd-nmgen xdbd-sysgen xdbd-nmgen-mq xdbd-nmgen-sdl xdbd-ham xdbd-stat: xdbd-errinfo

xdbd-sql-parser: xdbd-odbc
