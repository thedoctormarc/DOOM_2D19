#include "j1EnemyIMP.h"
#include "j1EntityFactory.h"

j1EnemyIMP::j1EnemyIMP(int posX, int posY, std::string name) : j1Enemy(posX, posY)
{
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - texture

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - texture
	entityTex = App->entityFactory->entityTextureMap.at(name);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - data
	type = ENEMY_IMP;
	this->name = name; 
	damage = 50; 
	maxLife = 50; 
	life = (float)maxLife;
	size.create(36, 49);
	speed = 150.75f;
	defaultSpeed = speed; 
	mass = 0.6f;
	gravityFactor = DEFAULT_GRAV * mass;
	tileDetectionRange = 10;
	cadenceValues.melee = 1200; 
	pathType = enemyPathType::ALL_ROUND; 
	deathDataAnimFx.hasSecondDeathAnim = true; 
	deathDataAnimFx.hasSecondDeathFx = false; 

	// jumpdata


	jumpInfo.jumpPower = 15.5f;
	jumpInfo.currenJumpPower = jumpInfo.jumpPower;
	jumpInfo.jumpIncrementFactor = .9f;
	jumpInfo.speedXIncrementJump = 2.3f;
	jumpInfo.speedXIncrementFall = 1.5f;
	jumpInfo.verticalIncrementFactor = 1.02f;
	
	state.movement.at(1) = eMovementState::FALL;   // testing purposes 

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - anims
	currentAnimation = &idle;
	idle.PushBack({ 46, 33, size.x, size.y });

	run.PushBack({ 46, 33, size.x, size.y });
	run.PushBack({ 42, 123, size.x + 2, size.y + 2});
	run.PushBack({ 45, 213, size.x -5, size.y + 4});
	run.speed = 1.5f; 
	run.loop = true; 


	death1.PushBack({ 44, 755, size.x + 6, size.y + 13});
	death1.PushBack({ 130, 755, size.x + 5, size.y + 10 });
	death1.PushBack({ 215, 755, size.x + 4, size.y + 5 });
	death1.PushBack({ 299, 755, size.x + 12, size.y - 3 });
	death1.PushBack({ 391, 755, size.x + 18, size.y - 27 });
	death1.speed = 1.3f; 
	death1.loop = false; 



	death2.PushBack({ 44, 850, size.x + 10, size.y + 10 });
	death2.PushBack({ 134, 850, size.x + 13, size.y + 12 });
	death2.PushBack({ 227, 850, size.x + 19, size.y + 12 });
	death2.PushBack({ 326, 850, size.x + 21, size.y + 12 });
	death2.PushBack({ 427, 850, size.x + 21, size.y - 5 });
	death2.PushBack({ 528, 850, size.x + 21, size.y - 15 });
	death2.PushBack({ 629, 850, size.x + 20, size.y - 18 });
	death2.PushBack({ 729, 850, size.x + 20, size.y - 31 });
	death2.speed = 1.9f;
	death2.loop = false;

	attack.PushBack({ 46, 396, size.x + 3, size.y});
	attack.PushBack({ 54, 489, size.x + 6, size.y });
	attack.PushBack({ 52, 577, size.x + 22, size.y + 2});
	attack.speed = 2.5f;
	attack.loop = false;
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - collider
	collider = App->collision->AddCollider({ (int)position.x, (int)position.y, (int)((float)size.x * spriteScale),(int)((float)size.y * spriteScale) }, COLLIDER_TYPE::COLLIDER_ENEMY, this);

}

j1EnemyIMP::~j1EnemyIMP()
{
}


bool j1EnemyIMP::Move(float dt)
{
	if (j1Enemy::Move(dt))
	{
		Jump();
		DoAttack(false); 
	}
		
	if (isPlayerOnMeleeRange())   
		DoAttack(true);



	return true; 
}


void j1EnemyIMP::Jump()
{


	BROFILER_CATEGORY("Enemy IMP jump", Profiler::Color::AntiqueWhite);


	// modified positions to check offset with platform

	iPoint lastTilePos = App->map->WorldToMap((int)previousPosition.x, (int)position.y) + iPoint(0, 1);
	iPoint tilePos = App->map->WorldToMap((int)position.x, (int)position.y) + iPoint(0, 1);

	if (onPlatform && App->entityFactory->player->onPlatform)  // both on plat 
	{
		bool enoughOffset = false;
		bool inSamePlatform = false; 

		if (lastPlatform == App->entityFactory->player->lastPlatform)
			inSamePlatform = true; 

		if (!inSamePlatform) 
		{


			for (const auto& col : App->entityFactory->player->collider->onCollisionWithMe)
			{
				if (col->hasCallback && col->callback->type == ENTITY_DYNAMIC || col->callback->type == ENTITY_STATIC)   // capture the platform player col  
				{

					iPoint platfTilePos = App->map->WorldToMap((int)col->callback->position.x, (int)col->callback->position.y) + iPoint(0, 1);

					if (pointingDir == RIGHT)
					{
						if (tilePos.x + App->map->WorldToMap(collider->rect.w, 0).x >= platfTilePos.x - jumpTriggerTileRange)               // check I got enough offset to jump and not collide with players' dest platform 
						{
							// check I was coming from that direction 

							if (lastTilePos.x < tilePos.x && lastTilePos.x + App->map->WorldToMap(collider->rect.w, 0).x  < platfTilePos.x - jumpTriggerTileRange)
							{
								enoughOffset = true;
							}

						}
					}
					else if (pointingDir == LEFT)
					{
						if (tilePos.x <= platfTilePos.x + App->map->WorldToMap(col->rect.w, 0).x + jumpTriggerTileRange)               // check I got enough offset to jump and not collide with players' dest platform 
						{
							if (lastTilePos.x > tilePos.x && lastTilePos.x > platfTilePos.x + App->map->WorldToMap(col->rect.w, 0).x + jumpTriggerTileRange)
							{
								enoughOffset = true;
							}
						}

					}


					if (App->entityFactory->player->collider->rect.y < collider->rect.y)    // player is above 
					{
						if (lastTilePos.x + App->map->WorldToMap(collider->rect.w, 0).x > platfTilePos.x - jumpTriggerTileRange - extraJumpOffset   // inside range, so it needs to go outside to have offset
							&& lastTilePos.x < platfTilePos.x + App->map->WorldToMap(col->rect.w, 0).x + jumpTriggerTileRange + extraJumpOffset)
						{
							if (!enoughOffset)   // repath to a position where the enemy would have enough ofsset to jump
							{
								if (position.x + collider->rect.w / 2 > col->rect.x + col->rect.w / 2)  
									targetPos.value.x = platfTilePos.x + App->map->WorldToMap(col->rect.w, 0).x + jumpTriggerTileRange + extraJumpOffset;
								else
									targetPos.value.x = platfTilePos.x - jumpTriggerTileRange - extraJumpOffset;
							
								targetPos.value.y = tilePos.y; 
								targetPos.type = TargetPos::targetPosType::X;
								state.path = ePathState::TEMPORAL_DEVIATION;
							}
						}
					}
					
				

				}
			}


			if (enoughOffset)
				doJump = true; 
		
		


		}
	}

}



void j1EnemyIMP::ResolvePathDeviation()
{
	doJump = true; 
}


void j1EnemyIMP::DoAttack(bool meleeRange)
{
	static uint lastTimeAttack = 0; 

	uint now = SDL_GetTicks();
	
	if (onPlatform)
	{
		if (state.combat == eCombatState::SHOOT)
		{
			if (currentAnimation != &attack)
				currentAnimation = &attack; 

			if (currentAnimation == &attack && currentAnimation->Finished() == true)  // hit is over 
			{
				if (isPlayerOnMeleeRange())   // do damage 
				{
					float ShotsPerSec = 1 / (cadenceValues.melee / 1000);
					App->entityFactory->DoDamagetoEntity(App->entityFactory->player, damage, ShotsPerSec);
					App->audio->PlayFx(name + "Attack"); 

				}

				
					currentAttackType = IMP_ATTACK_TYPE::NO_ATTACK_TYPE;     // retreat 
					state.combat = eCombatState::IDLE;
					speed = defaultSpeed;
			
				if (lastSpeed.IsZero())
					currentAnimation = &idle;   // go back to default anim depending on enemy speed
				else
					currentAnimation = &run;

			}

		}
		else if (state.combat == eCombatState::IDLE)
		{
			if (now >= lastTimeAttack + cadenceValues.melee)
			{
				if (meleeRange)
				{

					currentAttackType = IMP_ATTACK_TYPE::MELEE;
					state.combat = eCombatState::SHOOT;
					currentAnimation = &attack;
					currentAnimation->Reset();
					lastTimeAttack = now;

				}
		
			}
			
		}
	}
	

}


