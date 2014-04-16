--[[ INTRO --]]
function init()
	playtrough = 1
	groupList = nil
	--Transmission():setFace("Daid1"):setText("Welcome heroes|of earth"):top():transmissionDone("intro2")
	Delay():time(0.1):call('launchBonus')
end
function intro2()
	Transmission():setFace("Daid1"):setText("You misssion is to|destroy the|M corperation"):top():transmissionDone("intro3")
end
function intro3()
	Transmission():setFace("Daid1"):setText("It will be hazardous|so take care"):top():transmissionDone("intro4")
end
function intro4()
	Transmission():setFace("Jaime1"):setText("INCOMMING ENEMIES"):top():transmissionDone("launchRound1")
end
--[[ ROUND 1 --]]
function launchRound1()
	setupNewRound()
	local w = ListWatcher():delay(0.5):allDestroyed("round1Finished")
	local g = EnemyGroup()
	w:add(g)
	g:add(BasicEnemy():setTargetPosition(150, 70));
	g:add(BasicEnemy():setTargetPosition(170, 70));
	table.insert(groupList, g)
end

--[[ ROUND 1 finished --]]

function round1Finished()
	Transmission():setFace("Daid1"):setText("Awesome job|you managed|to take out the enemy"):top():transmissionDone("round1Finished2")
end
function round1Finished2()
	Transmission():setFace("Jaime1"):setText("Oh No|More enemies inbound!"):top():transmissionDone("launchRound2")
end

--[[ ROUND 2 --]]

function launchRound2()
	local w = ListWatcher():delay(0.5):allDestroyed("round2Finished")
	setupNewRound()
	local g = EnemyGroup()
	w:add(g)
	for n=0,7 do
		g:add(BasicEnemy():giveShield():setTargetPosition(160 - 4 * 20 + n * 20, 50));
	end
	table.insert(groupList, g)

	g = EnemyGroup()
	w:add(g)
	for n=0,9 do
		g:add(BasicEnemy():setTargetPosition(160 - 5 * 20 + n * 20, 70));
	end
	table.insert(groupList, g)

	g = EnemyGroup()
	w:add(g)
	for n=0,3 do
		g:add(BurstShotEnemy():setTargetPosition(160 - 5 * 20 + n * 20, 90));
	end
	table.insert(groupList, g)

	g = EnemyGroup()
	w:add(g)
	for n=0,3 do
		g:add(BurstShotEnemy():setTargetPosition(160 + 4 * 20 - n * 20, 90));
	end
	table.insert(groupList, g)
end

--[[ ROUND 2 finished --]]

function round2Finished()
	Transmission():setFace("Daid1"):setText("These M fighters|are better equiped|then us"):top():transmissionDone("round2Finished2")
end
function round2Finished2()
	Transmission():setFace("Daid1"):setText("Good thing they|are stupid."):top():transmissionDone("round2Finished3")
end
function round2Finished3()
	Transmission():setFace("Daid1"):setText("Better destroy some|smaller 3D printers|and grab the upgrades"):top():transmissionDone("round2Finished4")
end
function round2Finished4()
	local w = ListWatcher():delay(2.0):allDestroyed("round2Finished5")
	if playtrough > 1 then
		--On the 2nd playtrough only give 1 powerup instead of 2.
		if random(0, 100) < 50 then
			w:add(PowerupCarrier():setDirection( 1))
		else
			w:add(PowerupCarrier():setDirection(-1))
		end
	else
		w:add(PowerupCarrier():setDirection( 1))
		w:add(PowerupCarrier():setDirection(-1))
	end
end
function round2Finished5()
	Transmission():setFace("Daid1"):setText("Large object inbound"):top():transmissionDone("round2Finished6")
end
function round2Finished6()
	Transmission():setFace("Jaime1"):setText("Watch out!|It is the|Replicator Z18"):top():transmissionDone("round2Finished7")
end
function round2Finished7()
	Transmission():setFace("Daid1"):setText("It does not|seem to do|anything"):top():transmissionDone("round2Finished8")
	launchRound3()	--Launch the round early, the ReplicatorZ18 is slow anyhow.
end
function round2Finished8()
	Transmission():setFace("Daid1"):setText("Maybe the firmware|is not installed"):top():transmissionDone("round2Finished9")
end
function round2Finished9()
	Transmission():setFace("Jaime1"):setText("It is on a crash|course towards us"):top():transmissionDone("round2Finished10")
end
function round2Finished10()
	Transmission():setFace("Jaime1"):setText("Blast your way|trough it!"):top()
end

--[[ ROUND 3 --]]

function launchRound3()
	local w = ListWatcher():allDestroyed("round3Finished")
	local r = ReplicatorZ18()
	w:add(r)
	if playtrough > 1 then
		r:setSpeed(20)
	end
end

--[[ ROUND 3 finished --]]

function round3Finished()
	Transmission():setFace("Daid1"):setText("More enemies inbound."):top():transmissionDone("Delay():time(0.5):call('launchRound4')")
end

function launchRound4()
	local w = ListWatcher():delay(0.5):allDestroyed("round4Finished")
	
	setupNewRound()
	local g = EnemyGroup()
	w:add(g)
	for n=0,3 do
		g:add(BasicEnemy():giveShield():setTargetPosition(160 - 4 * 20 + n * 20, 50));
	end
	g:flyInBy(320, -20, 160, 160);
	table.insert(groupList, g)

	g = EnemyGroup()
	w:add(g)
	for n=4,7 do
		g:add(BasicEnemy():giveShield():setTargetPosition(160 - 4 * 20 + n * 20, 50));
	end
	g:flyInBy(0, -20, 160, 160);
	table.insert(groupList, g)

	g = EnemyGroup()
	w:add(g)
	for n=0,1 do
		g:add(BurstShotEnemy():giveShield():setTargetPosition(40 + n * 20, 100));
	end
	g:flyInBy(160, -20, 160, 160);
	table.insert(groupList, g)
	g = EnemyGroup()
	w:add(g)
	for n=0,1 do
		g:add(BurstShotEnemy():giveShield():setTargetPosition(320 - (40 + n * 20), 100));
	end
	g:flyInBy(160, -20, 160, 160);
	table.insert(groupList, g)
	
	w:add(Digitizer():setPosition(40, -50))
	w:add(Digitizer():setPosition(320 - 40, -50))
end

--[[ ROUND 4 finished --]]
function round4Finished()
	local w = ListWatcher():delay(2.0):allDestroyed("round4Finished2")
	if random(0, 100) < 50 then
		w:add(PowerupCarrier():setDirection( 1))
	else
		w:add(PowerupCarrier():setDirection(-1))
	end
end
function round4Finished2()
	Transmission():setFace("Jaime1"):setText("Watch out"):top():transmissionDone("round4Finished3")
end
function round4Finished3()
	Transmission():setFace("Jaime1"):setText("You are flying into|a field of abandoned|printers"):top():transmissionDone("launchRound5")
end
--[[ ROUND 5 --]]
function spawnOldPrinter()
	OldPrinter()
	if spawnPrinters then
		if playtrough > 1 then
			Delay():time(0.6):call("spawnOldPrinter")
		else
			Delay():time(0.8):call("spawnOldPrinter")
		end
	end
end
function launchRound5()
	Delay():time(5.0):call("delayedLaunchRound5")
	spawnPrinters = true
	spawnOldPrinter()
end
function delayedLaunchRound5()
	local w = ListWatcher():delay(1.5):allDestroyed("delayedLaunchRound5_2")
	w:add(Digitizer():setPosition(40, -50))
	w:add(Digitizer():setPosition(320 - 40, -50))
end
function delayedLaunchRound5_2()
	setupNewRound()
	local w = ListWatcher():delay(1.5):allDestroyed("delayedLaunchRound5_3")
	w:add(Digitizer():setPosition(40, -50))
	w:add(Digitizer():setPosition(160, -50))
	w:add(Digitizer():setPosition(320 - 40, -50))
	local g = EnemyGroup()
	w:add(g)
	for n=0,7 do
		g:add(BasicEnemy():setTargetPosition(160 - 4 * 20 + n * 20, 50));
	end
	g:flyInBy(160, -20, 160, 100);
	table.insert(groupList, g)
end
function delayedLaunchRound5_3()
	setupNewRound()
	local w = ListWatcher():allDestroyed("spawnPrinters = false; Delay():time(10.0):call('round5Finished')")
	w:add(Digitizer():setPosition(40, -50))
	w:add(Digitizer():setPosition(120, -50))
	w:add(Digitizer():setPosition(200, -50))
	w:add(Digitizer():setPosition(320 - 40, -50))
	local g = EnemyGroup()
	w:add(g)
	for n=0,7 do
		g:add(BasicEnemy():setTargetPosition(160 - 4 * 20 + n * 20, 50));
	end
	g:flyInBy(160, -20, 160, 100);
	table.insert(groupList, g)
	g = EnemyGroup()
	w:add(g)
	for n=0,7 do
		g:add(BasicEnemy():setTargetPosition(160 - 4 * 20 + n * 20, 70));
	end
	g:flyInBy(160, -20, 160, 120);
	table.insert(groupList, g)
end
--[[ ROUND 5 finished --]]
function round5Finished()
	Transmission():setFace("Jaime1"):setText("I am detecting a|huge ego on|the radar"):top():transmissionDone("round5Finished2")
end
function round5Finished2()
	Transmission():setFace("Daid1"):setText("Oh no!|It is him!"):top():transmissionDone("round5Finished3")
end
function round5Finished3()
	Transmission():setFace("Jaime1"):setText("Kill IT!!!"):top():transmissionDone("launchRound6")
end
--[[ ROUND 5 finished --]]
function launchRound6()
	BreEnemy():setDifficulty(playtrough):destroyed("round6Finished")
end
--[[ ROUND 6 finished --]]
function round6Finished()
	Transmission():setFace("Daid1"):setText("You did it|You saved the universe."):top():transmissionDone("round6Finished2")
end
function round6Finished2()
	Transmission():setFace("Henk1"):setText("Thank you|            |But our princess|is in another castle!"):top():transmissionDone("round6Finished3")
	SurfHenk()
end
function round6Finished3()
	playtrough = playtrough + 1
	Delay():time(3.0):call('launchBonus')
end

function launchBonus()
	BonusRound():finished("Delay():time(1.0):call('round1Finished2')")
end

--[[ Utility functions --]]
function setupNewRound()
	groupList = {}
	enemyDirection = 20.0
	enemyOffset = 0
	diveCountdown = random(2.0, 5.0)
end

--[[ Called each update, to manage flying in M logos --]]
function update(delta)
	if groupList ~= nil then
		if enemyOffset > 30 then
			enemyDirection = -math.abs(enemyDirection)
		end
		if enemyOffset < -30 then
			enemyDirection = math.abs(enemyDirection);
		end
		enemyOffset = enemyOffset + enemyDirection * delta;
		
		allowDive = true;
		allowFlyIn = true;
		count = 0;
		for _, g in ipairs(groupList) do
			if g:isValid() then
				count = count + 1
				g:setOffset(enemyOffset)
				if not g:isAll("CenterField") then
					allowDive = false;
					if not g:isAll("Outside") then
						allowFlyIn = false;
					end
				end
			end
		end
		if count < 1 then
			groupList = nil
			return
		end
		if allowFlyIn then
			for _ = 1, playtrough do
				g = groupList[math.random(#groupList)];
				if g:isValid() and g:isAll("Outside") then
					g:flyInBy(random(0, 320), -20, 160, 160);
				end
			end
		end

		if allowDive then
			if diveCountdown > 0 then
				diveCountdown = diveCountdown - delta;
			else
				g = groupList[math.random(#groupList)];
				if g:isValid() then
					diveCountdown = random(2.0, 5.0);
					g:dive(random(20, 300), 300);
				end
			end
		end
	end
end
