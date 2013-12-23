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
	Transmission():setFace("Jaime1"):setText("INCOMMING ENEMIES"):top():transmissionDone("launchRound")
end

function launchRound()
	groupList = {}
	enemyDirection = 20.0;
	enemyOffset = 0;
	diveCountdown = random(2.0, 5.0);

	g = EnemyGroup();
	for n=0,7 do
		g:add(BasicEnemy():giveShield():setTargetPosition(160 - 4 * 20 + n * 20, 50));
	end
	table.insert(groupList, g)

	g = EnemyGroup();
	for n=0,9 do
		g:add(BasicEnemy():setTargetPosition(160 - 5 * 20 + n * 20, 70));
	end
	table.insert(groupList, g)

	g = EnemyGroup();
	for n=0,3 do
		g:add(BurstShotEnemy():setTargetPosition(160 - 5 * 20 + n * 20, 90));
	end
	table.insert(groupList, g)

	g = EnemyGroup();
	for n=0,3 do
		g:add(BurstShotEnemy():setTargetPosition(160 + 4 * 20 - n * 20, 90));
	end

	table.insert(groupList, g)
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
			PowerupCarrier()
			groupList = nil
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
