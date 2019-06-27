#include "j1EntityLootWeapon.h"
#include "j1EntityFactory.h"

j1EntityLootWeapon::j1EntityLootWeapon(float posX, float posY, LOOT_TYPE loot_type, std::string name, weaponInfo weaponData) :j1EntityLoot(posX, posY, loot_type, name)
{

	useRenderFlip = true;
	defaultPointingDir = POINTING_DIR::RIGHT; 
	pointingDir = POINTING_DIR::RIGHT; 

	this->weaponData = weaponData; 
	this->weaponData.weaponState = WEAPON_STATE::AWAIT; 

	// define an offset from player, which may vary depending on weapon sprite

	this->weaponData.offsetFromPlayer = fPoint(21, 15);   // this is the default value 

	spriteScale = 0.65f;   // in the available sprites weapons are too big 

	switch (weaponData.weaponType)
	{
	case WEAPON_TYPE::CHAINSAW:

		size.create(62, 23); 
		section = {30, 0, size.x, size.y}; 
		
		// TODO: offset form player (when player is holding the weapon)

		break; 
	}

	collider = App->collision->AddCollider({ section.x, section.y, (int)((float)section.w*spriteScale), (int)((float)section.h* spriteScale) }, COLLIDER_TYPE::COLLIDER_LOOT, this);
	collider->SetPos(position.x, position.y); 

}

 
j1EntityLootWeapon::~j1EntityLootWeapon()
{

}

void j1EntityLootWeapon::PlaceMeWithPlayer()
{

	if(App->entityFactory->player->pointingDir == POINTING_DIR::RIGHT)
		position = App->entityFactory->player->position + weaponData.offsetFromPlayer;
	else if (App->entityFactory->player->pointingDir == POINTING_DIR::LEFT)
	{
		
		position = App->entityFactory->player->position + fPoint(App->entityFactory->player->size.x * App->entityFactory->player->spriteScale - weaponData.offsetFromPlayer.x - collider->rect.w, weaponData.offsetFromPlayer.y);

		if (App->entityFactory->player->state.movement.at(0) == MovementState::IDLE)
		{
			if (weaponData.extraOffsetXPlayerIdle == 0.f)
				weaponData.extraOffsetXPlayerIdle = App->entityFactory->player->collider->rect.w - App->entityFactory->player->lastPosCollider.w;

			position.x += weaponData.extraOffsetXPlayerIdle;

		}
		


			    // reposition to a new offset  
	}
		



	collider->SetPos(position.x, position.y);
 
	lastSpeed = App->entityFactory->player->lastSpeed; 
}

