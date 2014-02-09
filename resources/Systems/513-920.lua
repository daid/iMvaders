--System 513-920

function init()
	sun = Sun():setRadius(768):setPosition(0, 5000);
	---[[
	J4 = Planet():setName("Jamarkley IV"):setRadius(350):setOrbit(sun, 6000, 90);
	E1 = Planet():setName("Exskoth I"):setType("Planet2"):setRadius(32):setOrbit(J4, 800, 0):setDensity(4000000000);
	E2 = Planet():setName("Exskoth II"):setType("Planet2"):setRadius(32):setOrbit(J4, 800, 180):setDensity(4000000000);
	
	L2 = Planet():setName("Limporyen II"):setType("Planet2"):setRadius(512):setOrbit(sun, 3500, 0)
	L3 = Planet():setName("Limporyen III"):setType("Planet2"):setRadius(512):setOrbit(sun, 3500, 180)
	LM7 = Planet():setName("Levi-Montalcini VII"):setRadius(600):setOrbit(sun, 10000, 180)
	--]]
	AsteroidField():setOrbit(sun, 5000, 1)
end

function update()

end
