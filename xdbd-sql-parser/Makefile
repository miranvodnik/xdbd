
-include ../makefile.defs

COPTS = -g -Wall -c -fmessage-length=0 -fPIC $(UNIX_FLAGS)
CPPOPTS = -g -Wall -c -fmessage-length=0 -fPIC $(UNIX_FLAGS)
IDIR = -I. \
	-I$(PRODUCT_DIR)/xdbd \
	-I$(PRODUCT_DIR)/xdbd-include \
	-I$(PRODUCT_DIR)/xdbd-errinfo \
	-I$(PRODUCT_DIR)/xdbd-core \
	-I$(PRODUCT_DIR)/xdbd-msg \
	-I$(PRODUCT_DIR)/xdbd-job \
	-I$(PRODUCT_DIR)/xdbd-mpx \
	-I$(PRODUCT_DIR)/xdbd-stat \
	-I/usr/include/libxml2
CFLAGS = $(DEF_FLAGS) $(COPTS) $(IDIR)
CPPFLAGS = $(DEF_FLAGS) $(CPPOPTS) $(IDIR)
LOPTS = -shared
LIBS = -lxdbdmsg -lxdbdmpx -lxdbdodbc -lxdbdapi -lxdbdstat
LPATH = -L$(PRODUCT_DIR)/lib

OBJS = \
	./SqlSyntax.o \
	./XdbdSqlDeleteFunction.o \
	./XdbdSqlInsertFunction.o \
	./XdbdSqlParser.o \
	./XdbdSqlResultInfo.o \
	./XdbdSqlSelectFunction.o \
	./XdbdSqlUpdateFunction.o \
	./XdbdSqlCatalogFunction.o \
	./XdbdDeleteTriggerJob.o \
	./XdbdInsertTriggerJob.o \
	./XdbdUpdateTriggerJob.o \
	./XdbdXmlColumnInfo.o \
	./XdbdXmlDataBase.o \
	./XdbdXmlTableInfo.o \
	./XdbdTrigger.o \
	./XdbdTriggerParameter.o \
	./XdbdChkXmlDir.o \
	./XdbdChkXsdDir.o \
	./XdbdChkStatDir.o \
	./XdbdInitStatAdapter.o \
	./sql-select.lex.o \
	./sql-select.tab.o \
	./sql_and_expression.o \
	./sql_column.o \
	./sql_column_name.o \
	./sql_columns.o \
	./sql_columns_part.o \
	./sql_comp_expression.o \
	./sql_constant.o \
	./sql_delete_statement.o \
	./sql_eq_expression.o \
	./sql_is_eq_expression.o \
	./sql_eq_expressions_part.o \
	./sql_eq_expressions.o \
	./sql_expression_part.o \
	./sql_expression.o \
	./sql_ge_expression.o \
	./sql_gt_expression.o \
	./sql_insert_statement.o \
	./sql_le_expression.o \
	./sql_lt_expression.o \
	./sql_ne_expression.o \
	./sql_is_ne_expression.o \
	./sql_or_expression.o \
	./sql_like_expression.o \
	./sql_primary_expression.o \
	./sql_select_statement.o \
	./sql_statement.o \
	./sql_table_part.o \
	./sql_table.o \
	./sql_tables_part.o \
	./sql_tables.o \
	./sql_update_statement.o \
	./sql_values_part.o \
	./sql_values.o \
	./sql_catalog.o \
	./sql_set_statement.o \
	./sql_order_by.o \
	./sql_ordering.o \
	./sql_as_part.o \
	./sql_function_name.o \

all: libxdbdsqlparser.so

# Tool invocations
libxdbdsqlparser.so: $(OBJS) $(USER_OBJS)
	$(CXX) $(LOPTS) $(LPATH) -o $@ $(OBJS) $(LIBS)
	cp $@ ../lib

XdbdSqlParser.cpp: sql-select.lex.h sql-select.tab.h

sql-select.lex.c sql-select.lex.h: XdbdSelectStatement.l
	flex -L XdbdSelectStatement.l

sql-select.tab.c sql-select.tab.h: XdbdSelectStatement.y
	bison -lvd XdbdSelectStatement.y

# Other Targets
clean:
	-@rm -f $(OBJS) sql-select.lex.c sql-select.lex.h sql-select.tab.c sql-select.tab.h libxdbdsqlparser.so
