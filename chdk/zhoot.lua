--[[
@title shoot, increment zoom.
@chdk_version 1.4.1
@param z zoom incr +/-
@default z -1
@param ev Exposure Value
@default ev 0
]]

-- Enable Remote: false  Enable script start: false
-- (no conflict with USB usage)
-- Press Shutter to start script

-- switch to shooting node
if ( get_mode() == false ) then
	print("switching to shooting mode")
	set_record(1)
	while ( get_mode() == false ) do sleep(200) end
end

	set_zoom_speed(1)
	zmax=get_zoom_steps()

repeat
	-- wait for USB power
	print("Zoom:",get_zoom() )
	repeat
		sleep(50)
		if ( is_pressed("video") ) then
			return
		end
	until is_pressed("remote")

	press("shoot_half")
	repeat
		sleep(50)
	until get_shooting() == true

	av = get_av96()
	if ( ev == 0 ) then
		tv = get_tv96()
		ev = av + tv
	end

	-- print("A:T:E:",av,tv,ev)

	-- set TV
	set_tv96(ev-av)

	exp = get_exp_count()
	-- take exposure with "shoot"
	shoot()

	while (get_exp_count() == exp) do
		sleep(50)
	end

        znext=get_zoom()+z
        if (znext < 0 ) then
		print("zoom min")
                return
        end
        if (znext > zmax) then
		print("zoom max")
                return
        end

	set_zoom(znext)

	i=1
	while (get_zoom() ~= znext) do
		i = i + 1
		if  (zmax >= znext + i)  then
			set_zoom(znext+i)
		else
			if (znext - i >= 0 ) then
				set_zoom(znext-i)
			else
				break
			end
		end
		set_zoom(znext)
	end

until is_pressed("video")
