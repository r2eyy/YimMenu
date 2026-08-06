#include "max_penetration.hpp"

namespace big
{
	static const Hash explosive_weapons[] = {
		0x93E220BD,0xA284510B,0x4DD2DC56,0x2C3731D9,
		0x24B17070,0x787F0BB,0xB62D1F67,0xBFD21C91,
		0x47757124,0xBA45E8B8,0xAF113F99,0x63AB0442,
		0x060EC506,0xAB564B93,0xA0973D5E,0xFDBC8A50
	};

	bool max_penetration::is_explosive_weapon(Hash hash)
	{
		for (const auto& h : explosive_weapons)
			if (h == hash) return true;
		return false;
	}

	bool max_penetration::is_enabled()
	{
		return m_enabled;
	}

	void max_penetration::enable()
	{
		m_enabled = true;
	}

	void max_penetration::disable()
	{
		m_enabled = false;
	}

	void max_penetration::shoot_penetrating_bullet(Ped player, Vector3 start, Vector3 end, Hash weapon)
	{
		MISC::SHOOT_SINGLE_BULLET_BETWEEN_COORDS_IGNORE_ENTITY(
			start.x, start.y, start.z,
			end.x, end.y, end.z,
			200, true, weapon, player,
			true, false, -1.0, 0, false, player
		);
	}

	void max_penetration::damage_peds_in_path(Ped player, Vector3 start, Vector3 direction)
	{
		float len = sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
		if (len == 0) return;
		
		Vector3 dir = {direction.x / len, direction.y / len, direction.z / len};

		for (const auto& ped : entity::get_all_peds())
		{
			if (ped == player) continue;
			if (!ENTITY::DOES_ENTITY_EXIST(ped)) continue;
			if (ENTITY::IS_ENTITY_DEAD(ped, false)) continue;
			if (!PED::IS_PED_HUMAN(ped)) continue;

			Vector3 ped_pos = ENTITY::GET_ENTITY_COORDS(ped, true);
			Vector3 to_ped = {ped_pos.x - start.x, ped_pos.y - start.y, ped_pos.z - start.z};
			float t = to_ped.x * dir.x + to_ped.y * dir.y + to_ped.z * dir.z;

			if (t <= 0) continue;

			Vector3 closest = {
				start.x + dir.x * t,
				start.y + dir.y * t,
				start.z + dir.z * t
			};

			float dist = sqrt(
				(ped_pos.x - closest.x) * (ped_pos.x - closest.x) +
				(ped_pos.y - closest.y) * (ped_pos.y - closest.y) +
				(ped_pos.z - closest.z) * (ped_pos.z - closest.z)
			);

			if (dist < 5.0f)
			{
				float health = ENTITY::GET_ENTITY_HEALTH(ped);
				if (health > 0)
				{
					PED::SET_PED_SUFFERS_CRITICAL_HITS(ped, true);
					ENTITY::SET_ENTITY_HEALTH(ped, health - 80.0f, 0, 0);
				}
			}
		}
	}

	void max_penetration::tick()
	{
		if (!m_enabled) return;

		g_fiber_pool->queue_job([]
		{
			Ped player = PLAYER::PLAYER_PED_ID();
			if (!ENTITY::DOES_ENTITY_EXIST(player)) return;

			Hash weapon = WEAPON::GET_SELECTED_PED_WEAPON(player);
			if (weapon == 0) return;
			if (is_explosive_weapon(weapon)) return;

			if (!PAD::IS_CONTROL_PRESSED(0, 24)) return;

			Vector3 player_pos = ENTITY::GET_ENTITY_COORDS(player, true);
			Vector3 cam_rot = CAM::GET_GAMEPLAY_CAM_ROT(2);

			float pitch = cam_rot.x * 0.0174533f;
			float yaw = cam_rot.z * 0.0174533f;

			Vector3 aim_dir = {
				-sin(yaw) * cos(pitch),
				cos(yaw) * cos(pitch),
				sin(pitch)
			};

			Vector3 end_pos = {
				player_pos.x + aim_dir.x * 5000.0f,
				player_pos.y + aim_dir.y * 5000.0f,
				player_pos.z + aim_dir.z * 5000.0f
			};

			shoot_penetrating_bullet(player, player_pos, end_pos, weapon);
			damage_peds_in_path(player, player_pos, aim_dir);
		});
	}
}
