
local progs = {gol.get_prog(0), gol.get_prog(0), gol.get_prog(0)}
local t = gol.get_time()
local st = math.sin(t)
local ct = math.cos(t)
local tt = math.tan(t)
local phases = {st, ct, -ct, tt}

function translate_bot_to_top(vert, phase)
	if vert == nil then
		return
	end
	for i in pairs(vert) do
		if i % 2 == 0 then
			vert[i] = vert[i] - phase + 1
		end
	end
	return vert
end

for i in pairs(progs) do
	local vert = {
	-1.0, -1.0,
	-1.0,  1.0,
	 1.0,  1.0,
	 1.0,  1.0,
	 1.0, -1.0,
	-1.0, -1.0
	}
	if progs[i] then
		if i < #phases then
		--	vert = translate_bot_to_top(vert, phases[i])
		end
		if vert == nil then
			return
		end
		local surf = gol.update_vao(progs[i], vert)
	end
end

--[[function clamp(val, min, max)
	if val < min then
		return min
	elseif val > max then
		return max
	end
	return val
end--]]

