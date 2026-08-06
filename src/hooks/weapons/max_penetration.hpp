#pragma once
#include "natives.hpp"
#include "fiber_pool.hpp"
#include "gta_util.hpp"

namespace big
{
	class max_penetration
	{
	public:
		static void enable();
		static void disable();
		static void tick();
		static bool is_enabled();

	private:
		inline static bool m_enabled = false;
		
		static bool is_explosive_weapon(Hash hash);
		static void shoot_penetrating_bullet(Ped player, Vector3 start, Vector3 end, Hash weapon);
		static void damage_peds_in_path(Ped player, Vector3 start, Vector3 direction);
	};
}
