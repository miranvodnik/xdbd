xdbd-test "SELECT sys_intf_type, mn_intf_id, used_lm FROM sys_interface WHERE ((nodeid = ?) AND (sys_intf_id = ?))" 2000 130 >/dev/null 2>&1&
xdbd-test "DELETE FROM sys_interface WHERE nodeid = ? AND sys_intf_id = ?" 2000 130 >/dev/null 2>&1&
xdbd-test "INSERT INTO sys_interface (nodeid, sys_intf_id, sys_intf_type, mn_intf_id, used_lm) VALUES (?, ?, ?, ?, ?)" 2000 130 14 0 0 >/dev/null 2>>sys_interface.log&
xdbd-test "UPDATE sys_interface SET sys_intf_type = ?, mn_intf_id = ?, used_lm = ? WHERE ((nodeid = ?) AND (sys_intf_id = ?))" 15 0 0 2000 130 >/dev/null 2>&1&
