# CMake generated Testfile for 
# Source directory: /home/ishuena/heinz/heinz
# Build directory: /home/ishuena/heinz/heinz/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(heinz_mwcs_no_dc "/home/ishuena/heinz/heinz/test/run.py" "./heinz_mwcs_no_dc" "./check_mwcs_solution" "/home/ishuena/heinz/heinz/test/lymphoma.stp" "lymphoma.dimacs")
add_test(heinz_mwcs_dc "/home/ishuena/heinz/heinz/test/run.py" "./heinz_mwcs_dc" "./check_mwcs_solution" "/home/ishuena/heinz/heinz/test/lymphoma.stp" "lymphoma.dimacs")
add_test(heinz_mwcs_no_pre "/home/ishuena/heinz/heinz/test/run.py" "./heinz_mwcs_no_pre" "./check_mwcs_solution" "/home/ishuena/heinz/heinz/test/lymphoma.stp" "lymphoma.dimacs")
add_test(heinz_mwcs_mc "/home/ishuena/heinz/heinz/test/run.py" "./heinz_mwcs_mc" "./check_mwcs_solution" "/home/ishuena/heinz/heinz/test/lymphoma.stp" "lymphoma.dimacs")
add_test(heinz_pcst_no_dc "/home/ishuena/heinz/heinz/test/run.py" "./heinz_pcst_no_dc" "./check_pcst_solution" "/home/ishuena/heinz/heinz/test/i640-003.stp" "i640-003.dimacs")
add_test(heinz_pcst_dc "/home/ishuena/heinz/heinz/test/run.py" "./heinz_pcst_dc" "./check_pcst_solution" "/home/ishuena/heinz/heinz/test/i640-003.stp" "i640-003.dimacs")
add_test(heinz_pcst_no_pre "/home/ishuena/heinz/heinz/test/run.py" "./heinz_pcst_no_pre" "./check_pcst_solution" "/home/ishuena/heinz/heinz/test/i640-003.stp" "i640-003.dimacs")
add_test(heinz_pcst_mc "/home/ishuena/heinz/heinz/test/run.py" "./heinz_pcst_mc" "./check_pcst_solution" "/home/ishuena/heinz/heinz/test/i640-003.stp" "i640-003.dimacs")
add_test(heinz_rpcst_no_dc "/home/ishuena/heinz/heinz/test/run.py" "./heinz_rpcst_no_dc" "./check_pcst_solution" "/home/ishuena/heinz/heinz/test/i101M2.stp" "i101M2.dimacs")
add_test(heinz_rpcst_no_pre "/home/ishuena/heinz/heinz/test/run.py" "./heinz_rpcst_no_pre" "./check_pcst_solution" "/home/ishuena/heinz/heinz/test/i101M2.stp" "i101M2.dimacs")
add_test(heinz_rpcst_mc "/home/ishuena/heinz/heinz/test/run.py" "./heinz_rpcst_mc" "./check_pcst_solution" "/home/ishuena/heinz/heinz/test/i101M2.stp" "i101M2.dimacs")
