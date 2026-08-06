#include "max_penetration.hpp"

namespace big
{
	void max_penetration_tick()
	{
		if (!g_max_penetration) return;

		g_fiber_pool->queue_job([]
		{
			Ped player = PLAYER::PLAYER_PED_ID();
			if (!ENTITY::DOES_ENTITY_EXIST(player)) return;

			Hash weapon = WEAPON::GET_SELECTED_PED_WEAPON(player);
			if (weapon == 0) return;

			static const Hash explosives[] = {
				0x93E220BD,0xA284510B,0x2C3731D9,0x24B17070,
				0x787F0BB,0xBFD21C91,0x47757124,0xBA45E8B8,
				0xAF113F99,0x63AB0442,0x060EC506,0xAB564B93,
				0xA0973D5E,0xFDBC8A50
			};
			for (const auto& h : explosives)
				if (h == weapon) return;

			if (!PAD::IS_CONTROL_PRESSED(0, 24)) return;

			Vector3 pos = ENTITY::GET_ENTITY_COORDS(player, true);
			Vector3 cam = CAM::GET_GAMEPLAY_CAM_ROT(2);
			float pitch = cam.x * 0.0174533f;
			float yaw = cam.z * 0.0174533f;

			Vector3 dir = {-sin(yaw)*cos(pitch), cos(yaw)*cos(pitch), sin(pitch)};
			Vector3 end = {pos.x+dir.x*5000, pos.y+dir.y*5000, pos.z+dir.z*5000};

			MISC::SHOOT_SINGLE_BULLET_BETWEEN_COORDS_IGNORE_ENTITY(
				pos.x,pos.y,pos.z, end.x,end.y,end.z,
				200,true,weapon,player,true,false,-1.0,0,false,player);
		});
	}
}
