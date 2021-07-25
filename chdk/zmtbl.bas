@title Zoom Table
@chdk_version 1.4.1

rem go to REC mode, lens out
rem get_mode returns 0 for record, 1 for play and 2 for movie
while get_mode <> 0
	set_record 1
 	sleep 100
wend

rem log output goes to CHDK/LOG/LOG_0001.TXT
print_screen 1

print "int zoomStep = 0;"
print "int zoomInOut = 1;"
print "double zoomConstant;"
print "const int zoomFocal[] = {"

s = get_zoom_steps - 1
for n=0 to s

	set_zoom n

	i = 1
	while get_zoom <> n
		
                i = i + 1
                if  s >= n + i then
                        set_zoom n+i
                else
                        if n - i >= 0 then
                                set_zoom n-i
                        else
                                break
                        endif
                endif
                set_zoom n
        wend

	f = get_focal_length

	print f;","

next n

print "0"
print "};"

print_screen 0

end

