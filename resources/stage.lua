--[[ INTRO --]]
function init()
	groupList = nil
	Transmission():setFace("Daid1"):setText("Welcome heroes|of earth"):top():transmissionDone("intro2")
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
	w = ListWatcher():delay(0.5):allDestroyed("round1Finished")
	g = EnemyGroup()
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
	w = ListWatcher():delay(0.5):allDestroyed("round2Finished")
	setupNewRound()
	g = EnemyGroup()
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
	w = ListWatcher():delay(2.0):allDestroyed("round2Finished5")
	w:add(PowerupCarrier():setDirection( 1))
	w:add(PowerupCarrier():setDirection(-1))
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
	Transmission():setFace("Jaime1"):setText("Blast your way|trough it!"):top():transmissionDone("Delay():time(25):call('round3joke')")
end

function round3joke()
	Transmission():setFace("Daid1"):setText("This has to be the|lamest enemy in|this game."):bottom()
end

--[[ ROUND 3 --]]

function launchRound3()
	w = ListWatcher():allDestroyed("round3Finished")
	w:add(ReplicatorZ18())
end

--[[ ROUND 3 finished --]]

function round3Finished()
	Transmission():setFace("Daid1"):setText("More enemies inbound."):transmissionDone("Delay():time(0.5):call('launchRound4')")
end

function launchRound4()
	w = ListWatcher():delay(0.5):allDestroyed("round4Finished")
	
	setupNewRound()
	g = EnemyGroup()
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
	Transmission():setFace("Jaime1"):setText("I am detecting a|huge ego on the radar!"):top():transmissionDone("round4Finished2")
end
function round4Finished2()
	Transmission():setFace("Daid1"):setText("Oh no!|It is him!"):top():transmissionDone("round4Finished3")
end
function round4Finished3()
	Transmission():setFace("Jaime1"):setText("Kill IT!!!"):top():transmissionDone("round4Finished4")
end
function round4Finished4()
	BreEnemy():destroyed("intro4")
end

function setupNewRound()
	groupList = {}
	enemyDirection = 20.0
	enemyOffset = 0
	diveCountdown = random(2.0, 5.0)
end

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
			g = groupList[math.random(#groupList)];
			if g:isValid() and g:isAll("Outside") then
				g:flyInBy(random(0, 320), -20, 160, 160);
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
