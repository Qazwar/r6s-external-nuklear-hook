#pragma once
#include "imports.h"

namespace globals {
	bool aimtab = true;
	bool visualtab = false;
	bool misctab = false;

	bool esp = false;
	bool clip = false;
	bool recoil = false;
	bool spread = false;
	bool damage = false;
	bool fov = false;
	bool aimbot = false;
	bool silent = false;
	bool glow = false;
	bool flash = false;
	bool impact = false;
	bool unlock = false;
	bool invisible = false;

	bool debug = false;

	float aimbotfov = 30.0f;
	float spreadmultiplier = 0.0f;
	float recoilmultiplier = 0.0f;
	int damagemultiplier = 0.0f;
	float customfov = 90.0f;
	float fovW = 0.900f;
	float fovP = 1.550f;

	uint64_t window_width = 300;
	uint64_t window_height = 425;

	static IDXGISwapChain* swap_chain;
	static ID3D11Device* device;
	static ID3D11DeviceContext* context;
	static ID3D11RenderTargetView* rt_view;
}

namespace offsets {
	// game manager
	constexpr uintptr_t game_manager = 0x51cb748;
	constexpr uintptr_t entity_count = 0x1d0;
	constexpr uintptr_t entity_list = 0x1c8;

	constexpr uintptr_t entity_component = 0xd8;
	constexpr uintptr_t entity_component_list = 0x8;
	constexpr uintptr_t entity_marker_vt_offset = 0x37bba90;
	constexpr uintptr_t entity_marker_enabled_offset = 0x530;

	constexpr uintptr_t  offset_entity_head = 0x180;
	constexpr uintptr_t  offset_entity_feet = 0x1d0;

	constexpr uintptr_t  offset_entity_skeleton = 0x20;
	constexpr uintptr_t  offset_skeleton_position = 0x6c0;
	constexpr uintptr_t  offset_skeleton_headposition = 0x660;
	constexpr uintptr_t  offset_entitypawn_feet = 0x6c0;

	constexpr uintptr_t entity_health = 0x140;
	constexpr uintptr_t entity_head = 0x660;
	constexpr uintptr_t entity_pelvis = 0xfa0;
	constexpr uintptr_t entity_feet = 0x6c0;
	constexpr uintptr_t entity_rhand = 0x680;

	constexpr uintptr_t entity_replication = 0xa8;
	constexpr uintptr_t entity_replication_team = 0x19e;
	constexpr uintptr_t entity_replication_name = 0x1c8;

	constexpr uintptr_t  offset_entity_pawn = 0x20;
	constexpr uintptr_t  offset_maincomponent_espchain = 0x78;
	constexpr uintptr_t  offset_espchain_write1 = 0x532;
	constexpr uintptr_t  offset_espchain_write2 = 0x534;

	constexpr uintptr_t  offset_espchain_check = 0x531;

	// renderer manager
	constexpr uintptr_t renderer_manager = 0x51b8988;
	constexpr uintptr_t game_renderer_deref = 0x0;

	// status manager
	constexpr uintptr_t status_manager = 0x51cb6e0;
	constexpr uintptr_t status_manager_container = 0x68;
	constexpr uintptr_t status_manager_localentity = 0x28;

	constexpr uintptr_t  offset_status_manager_localentity = 0x28;
	constexpr uintptr_t  offset_status_manager_container = 0x370;

	// camera manager
	constexpr uintptr_t camera_manager = 0x51a6168;
	constexpr uintptr_t camera_enginelink = 0x120;
	constexpr uintptr_t camera_engine = 0x1d8;
	constexpr uintptr_t camera_engine_camera = 0x08;

	constexpr uintptr_t camera_right = 0xd0;
	constexpr uintptr_t camera_up = 0xe0;
	constexpr uintptr_t camera_forward = 0xf0;
	constexpr uintptr_t camera_translation = 0x100;
	constexpr uintptr_t camera_fovx = 0x110;
	constexpr uintptr_t camera_fovy = 0x124;

	// network manager
	constexpr uintptr_t network_manager = 0x51b8958;

	// glow manager
	constexpr uintptr_t glow_manager = 0x5e3f4e8;

	// profile manager, interface manager, player manager
	constexpr uintptr_t profile_manager = 0x51b87b0;

	// fov manager
	constexpr uintptr_t fov_manager = 0x51cb730;

	// spoof specate manager
	constexpr uintptr_t spoof_spectate = 0x51b8840;

	// round manager
	constexpr uintptr_t round_manager = 0x51b8820;

	// unlocks manager
	constexpr uintptr_t unlocks_manager = 0x132f4c5;

	// trigger manager
	constexpr uintptr_t  trigger_manager = 0x51cb748;
	constexpr uintptr_t  trigger_1 = 0x50;
	constexpr uintptr_t  trigger_2 = 0x80;
	constexpr uintptr_t  trigger_3 = 0x58;
	constexpr uintptr_t  trigger_4 = 0x418;
	constexpr uintptr_t  trigger_5 = 0x304;
}

class RainbowSix {
public:
	uintptr_t base_address;

	RainbowSix() {
		process_id = get_process_id("RainbowSix.exe");
		base_address = get_module_base_address("RainbowSix.exe");
	}

	bool enemy_entities(std::vector<uintptr_t>& players) {
		unsigned short local_team = entity_team(local_entity());
		std::cout << "[+] Local team: " << local_team << std::endl;
		unsigned int count = entity_team(base_address);
		std::cout << "[+] count: " << count << std::endl;

		if (count > 255)
			return false;

		for (unsigned int i = 0; i < count; i++)
		{
			uintptr_t target_player = entity_id(i);

			if (!target_player)
				continue;

			const auto lentity = local_entity();

			if (!lentity)
				continue;

			if (target_player == lentity)
				continue;

			if (entity_team(target_player) == local_team)
				continue;

			players.push_back(target_player);
		}

		return true;
	}

	uintptr_t entity_id(unsigned int id) {
		uintptr_t game_manager = Read<uintptr_t>(base_address + offsets::game_manager);
		if (!game_manager)
			return NULL;

		uintptr_t entity_list = Read<uintptr_t>(game_manager + offsets::entity_list);
		if (!entity_list)
			return NULL;

		uintptr_t entity = Read<uintptr_t>(entity_list + (sizeof(PVOID) * id));
		if (!entity)
			return NULL;

		return entity;
	}

	std::uint8_t entity_team(uintptr_t entity) {

		if (!entity)
			return 0xFF;

		uintptr_t replication = Read<uintptr_t>(entity + offsets::entity_replication);
		std::cout << "replication " << replication << std::endl;
		if (!replication) {
			return 0xFF;
		}

		unsigned long online_team_id = Read<unsigned long>(replication + offsets::entity_replication_team);
		std::cout << "online_team_id " << online_team_id << std::endl;
		if (!online_team_id) {
			return 0xFF;
		}

		return online_team_id;
	}

	uintptr_t local_entity() {

		uintptr_t status_manager = Read<uintptr_t>(base_address + offsets::status_manager);

		if (!status_manager)
			return 0;

		uintptr_t entity_container = Read<uintptr_t>(status_manager + offsets::status_manager_container);

		if (!entity_container)
			return 0;

		entity_container = Read<uintptr_t>(entity_container);
		if (!entity_container)
			return 0;

		entity_container = Read<uintptr_t>(entity_container + offsets::offset_status_manager_localentity);

		if (!entity_container)
			return 0;

		return entity_container;
	}

	unsigned long entity_count() {
		uintptr_t game_manager = Read<uintptr_t>(base_address + offsets::game_manager);
		if (!game_manager)
			return NULL;

		return Read<unsigned long>(game_manager + offsets::entity_count) & 0x3fffffff;
	}

	bool glow() {
		if (globals::glow)
		{
			uint64_t glowmanager = Read<uint64_t>(base_address + offsets::glow_manager);

			uint64_t glowmanager1 = Read<uint64_t>(glowmanager + 0xB8);

			if (!glowmanager)
				return false;
			if (!glowmanager1)
				return false;

			if (globals::glow) {
				std::random_device seeder;
				std::ranlux48 gen(seeder());
				std::uniform_int_distribution<int>  uniform_0_255(0, 255);

				Write<float>(glowmanager1 + 0x110, uniform_0_255(gen)); // red
				Write<float>(glowmanager1 + 0x114, uniform_0_255(gen)); // green
				Write<float>(glowmanager1 + 0x118, uniform_0_255(gen)); // blue
				Write<float>(glowmanager1 + 0x13C, 3); // opcacity.
				Write<float>(glowmanager1 + 0x130, 0); // dist.
				Write<float>(glowmanager1 + 0x130 + 0x4, 0); // dist.
			}
			else {
				Write<float>(glowmanager1 + 0x110, 0); // red
				Write<float>(glowmanager1 + 0x114, 0); // green
				Write<float>(glowmanager1 + 0x118, 0); // blue
				Write<float>(glowmanager1 + 0x13C, 0); // opcacity.
				Write<float>(glowmanager1 + 0x130, 0); // dist.
				Write<float>(glowmanager1 + 0x130 + 0x4, 0); // dist.
			}
		}
		return true;
	}

	bool patch_terminate()
	{
		HMODULE module = GetModuleHandle(("kernelbase.dll"));

		if (!module)
			return false;

		uint64_t r6_module = get_module_base_address("kernelbase.dll");

		if (!r6_module)
			return false;

		uint64_t api = reinterpret_cast<uint64_t>(GetProcAddress(module, ("TerminateProcess"))) - reinterpret_cast<uint64_t>(module) + r6_module;

		printf("[+] Cheeto Threads: %llx\n", api);

		if (change_protection(process_id, api, PAGE_EXECUTE_READWRITE, 0x64) == 0)
		{
			uint8_t ret_stub[] = { 0xC3, 0x90, 0x90, 0x90 };

			WriteVirtualMemoryRaw(api, reinterpret_cast<uintptr_t>(ret_stub), sizeof(ret_stub));
			change_protection(process_id, api, PAGE_EXECUTE_READ, 0x64);
		}

		return true;
	}

	bool all_unlocks()
	{
		if (globals::unlock) {
			/* uint64_t instruction = this->base_address + OFFSET_UNLOCKS;
			 uint8_t operand = Read<uint8_t>( instruction + 3 );

			 if ( change_protection( PID, instruction, PAGE_EXECUTE_READWRITE, 0x64 ) == 0 )
			 {
				 uint8_t unlocks_on[]  = { 0xC6, 0x47, 0x38, 0x00 };
				 uint8_t unlocks_off[] = { 0xC6, 0x47, 0x38, 0x01 };

				 if ( enabled )
					 WriteVirtualMemoryRaw( instruction, reinterpret_cast<uintptr_t>( unlocks_on ), sizeof( unlocks_on ) );
				 else
					 WriteVirtualMemoryRaw( instruction, reinterpret_cast<uintptr_t>( unlocks_off ), sizeof( unlocks_off ) );

				 change_protection( PID, instruction, PAGE_EXECUTE_READ, 0x64 );
			 }*/

			auto instruction = this->base_address + globals::unlock;

			if (change_protection(process_id, instruction, PAGE_EXECUTE_READWRITE, 4) == 0)
			{
				Write<std::uint8_t>(instruction + 3, globals::unlock ? 0 : 1);

				change_protection(process_id, instruction, PAGE_EXECUTE_READ, 4);
			}
		}
		return true;
	}

	void invisible()
	{
		if (globals::invisible) {
			auto profile_manager = Read<uintptr_t>(this->base_address + offsets::profile_manager);

			if (!profile_manager)
				return;

			auto entity_list = Read<uintptr_t>(Read<uintptr_t>(profile_manager + 0x68));

			if (!entity_list)
				return;

			auto operator_list = Read<uintptr_t>(entity_list + 0x2d0);

			if (!operator_list)
				return;

			for (auto i = 0; i < 100; i++)
			{
				auto coperator = Read<uintptr_t>(operator_list + (i * 8));

				if (!coperator)
					continue;

				auto headgear = Read<uintptr_t>(coperator + 0x68);

				if (!headgear)
					continue;

				Write<uintptr_t>(coperator + 0x98, headgear);
			}
		}
	}

	bool esp()
	{
		if (globals::esp) {
			uintptr_t game_manager = Read<uintptr_t>(base_address + offsets::game_manager);
			if (!game_manager)
				return NULL;

			uintptr_t entity_list = Read<uintptr_t>(game_manager + offsets::entity_list);
			int entity_count = Read<DWORD>(game_manager + offsets::entity_count) & 0x3fffffff;
			if (entity_count == NULL) return false;

			for (int i = 0; i < entity_count; i++)
			{
				uintptr_t entity_object = Read<uintptr_t>(entity_list + i * 0x8);

				uintptr_t addr2 = Read<uintptr_t>(entity_object + 0x28);
				if (addr2 == NULL) continue;
				uintptr_t addr3 = Read<uintptr_t>(addr2 + 0xD8);
				if (addr3 == NULL) continue;

				for (auto current_component = 0x80; current_component < 0xf0; current_component += sizeof(std::uintptr_t))
				{
					uintptr_t addr4 = Read<uintptr_t>(addr3 + current_component);
					if (addr4 == NULL) continue;

					if (Read<uintptr_t>(addr4) != (base_address + offsets::entity_marker_vt_offset)) //vt marker
						continue;

					Write<BYTE>(addr4 + 0x532, globals::esp);
					Write<BYTE>(addr4 + 0x534, globals::esp);
				}
			}
		}
	}

	bool fov()
	{
		if (globals::fov) {
			uint64_t OFFSET_FOV = Read<uint64_t>(base_address + offsets::fov_manager);

			if (!OFFSET_FOV)
				return false;

			uint64_t weapon_fov = Read<uint64_t>(OFFSET_FOV + 0x28);
			uint64_t class1_unknown = Read<uint64_t>(weapon_fov + 0x0);

			Write<float>(class1_unknown + 0x3C, globals::fovW);
			Write<float>(class1_unknown + 0x38, globals::fovP);
		}
	}

	bool outline()
	{
		uint64_t outline = Read<uint64_t>(base_address + 0x50660B0);

		if (!outline)
			return false;

		uint64_t teamo = Read<uint64_t>(outline + 0x88);
		uint64_t teamou = Read<uint64_t>(teamo + 0x38);
		uint64_t teamout = Read<uint64_t>(teamou + 0x68);

		Write<unsigned int>(teamout + 0x20, 5);

		return true;
	}

	bool no_recoil()
	{
		if (globals::recoil) {
			uintptr_t LpVisualCompUnk = Read<uintptr_t>(local_entity() + 0x78);

			if (!LpVisualCompUnk)
				return false;

			uintptr_t LpWeapon = Read<uintptr_t>(LpVisualCompUnk + 0xC8);

			if (!LpWeapon)
				return false;

			uintptr_t LpCurrentDisplayWeapon = Read<uintptr_t>(LpWeapon + 0x208);

			if (!LpCurrentDisplayWeapon)
				return false;

			float recoilNumberOne;
			float recoilNumberTwo;

			if (globals::recoil == true) {
				recoilNumberOne = Read<float>(LpCurrentDisplayWeapon + 0x50); //spread
				recoilNumberTwo = Read<float>(LpCurrentDisplayWeapon + 0xB0); //recoil

				Write<float>(LpCurrentDisplayWeapon + 0x50, 0.0f); //spread
				Write<float>(LpCurrentDisplayWeapon + 0xB0, 0.0f); //recoil
			}
			else {

				Write<float>(LpCurrentDisplayWeapon + 0x50, recoilNumberOne); //spread
				Write<float>(LpCurrentDisplayWeapon + 0xB0, recoilNumberTwo); //recoil
			}
		}
		return true;
	}

	bool no_flash() {
		if (globals::flash) {
			uint8_t Activate = 0;
			uintptr_t lpEventManager = Read<uintptr_t>(local_entity() + 0x30);
			uintptr_t lpFxArray = Read<uintptr_t>(lpEventManager + 0x30);
			const UINT uStunIndex = 5;
			uintptr_t lpFxStun = Read<uintptr_t>(lpFxArray + (uStunIndex * sizeof(PVOID)));
			Write<uint8_t>(lpFxStun + 0x40, Activate);
		}
		return true;
	}

	void no_clip() {
		if (globals::clip) {
			uintptr_t networkManager = Read<uintptr_t>(base_address + offsets::network_manager); //network M 
			uintptr_t noclipPtr = Read<uintptr_t>(networkManager + 0xF8);

			noclipPtr = Read<uintptr_t>(noclipPtr + 0x8); //no clip 2

			noclipPtr += 0x530; //no clip 3

			Vector3 emptyVector = Vector3(0, 0, 0);
			Vector3 defaultVector = Vector3(0.0001788139343f, 0.0001788139343f, 0.03051757626f);

			Write<Vector3>(noclipPtr, (globals::clip ? emptyVector : defaultVector));
		}
	}

	uintptr_t get_closest_enemy()
	{
		const auto getViewangle = [](std::uintptr_t entity)
		{
			auto r1 = Read<uintptr_t>(entity + 0x20);
			auto r2 = Read<uintptr_t>(r1 + 0x1170);
			return Read<Vector4>(r2 + 0xc0);
		};

		std::uintptr_t resultant_entity = 0;
		static auto resultant_fov = 360.f;

		for (auto i = 0ul; i < this->entity_count(); i++)
		{
			const auto entity = this->entity_id(i);

			const auto health = this->entity_health(entity);

			if (health <= 0 || health > 200)
				continue;

			const auto lentity = local_entity();

			if (this->entity_team(lentity) == this->entity_team(entity))
				continue;

			const auto fov_result = closest_to_fov(this->entity_head(lentity), this->entity_head(entity), calculate_euler(getViewangle(lentity)));

			if (fov_result < resultant_fov)
			{
				resultant_fov = fov_result;
				resultant_entity = entity;
			}
		}

		return resultant_entity;
	}

	uintptr_t get_closest_enemy_to_local_head() {
		uintptr_t game_manager = Read<uintptr_t>(base_address + offsets::game_manager);
		if (!game_manager)
			return NULL;

		int num = Read<DWORD>(game_manager + 0x1D0);

		Vector3 CurrentViewAngles = entity_head(local_entity());
		uintptr_t entity_object;
		float bestDelta = FLT_MAX;
		uintptr_t BestEntity;
		uint64_t EntList = Read<uint64_t>(game_manager + 0x1C8);

		for (int i = 0; i < num; i++) {
			entity_object = Read<uint64_t>(EntList + i * 0x8);

			Vector3 angles = calc_angle(entity_head(local_entity()), entity_head(entity_object));

			(angles - CurrentViewAngles).clamp();

			float Delta = angles.y;

			if (Delta < bestDelta && local_entity() != entity_object) {
				bestDelta = Delta;
				BestEntity = entity_object;
				return BestEntity;
			}
		}
	}

	Vector3 entity_feet(uintptr_t entity) {
		uint64_t addr1 = Read<uint64_t>(entity + offsets::offset_entity_pawn);
		return Read<Vector3>(addr1 + offsets::offset_entitypawn_feet);
	}

	Vector3 entity_head(uintptr_t player)
	{
		uint64_t pSkeleton = Read<uint64_t>(player + 0x20);

		if (!pSkeleton)
			return Vector3();

		return Read<Vector3>(pSkeleton + offsets::offset_skeleton_headposition); // Ox6A0
	}

	bool world_to_screen(Vector3 position, Vector2* screen)
	{
		uintptr_t camera_manager = Read<uintptr_t>(base_address + offsets::camera_manager);
		if (!camera_manager)
			return false;

		Vector3 temp = position - view_translation();

		float x = temp.Dot(view_right());
		float y = temp.Dot(view_up());
		float z = temp.Dot(view_forward() * -1.f);

		RECT desktop;
		const HWND hDesktop = GetDesktopWindow();
		GetWindowRect(hDesktop, &desktop);
		int horizontal = desktop.right;
		int vertical = desktop.bottom;

		screen->x = (horizontal / 2.f) * (1.f + x / view_fovx() / z);
		screen->y = (vertical / 2.f) * (1.f - y / view_fovy() / z);

		return z >= 1.0f ? true : false;
	}

	Vector3 view_translation()
	{
		uintptr_t camera_manager = Read<uintptr_t>(base_address + offsets::camera_manager);
		if (!camera_manager)
			return Vector3();

		return Read<Vector3>(camera_manager + offsets::camera_translation);
	}
	Vector3 view_right()
	{
		uintptr_t camera_manager = Read<uintptr_t>(base_address + offsets::camera_manager);
		if (!camera_manager)
			return Vector3();

		return Read<Vector3>(camera_manager + offsets::camera_right);
	}
	Vector3 view_up()
	{
		uintptr_t camera_manager = Read<uintptr_t>(base_address + offsets::camera_manager);
		if (!camera_manager)
			return Vector3();

		return Read<Vector3>(camera_manager + offsets::camera_up);
	}

	Vector3 view_forward()
	{
		uintptr_t camera_manager = Read<uintptr_t>(base_address + offsets::camera_manager);
		if (!camera_manager)
			return Vector3();

		return Read<Vector3>(camera_manager + offsets::camera_forward);
	}

	float view_fovx()
	{
		uintptr_t camera_manager = Read<uintptr_t>(base_address + offsets::camera_manager);
		if (!camera_manager)
			return 0.0f;

		return Read<float>(camera_manager + offsets::camera_fovx);
	}

	float view_fovy()
	{
		uintptr_t camera_manager = Read<uintptr_t>(base_address + offsets::camera_manager);
		if (!camera_manager)
			return 0.0f;

		return Read<float>(camera_manager + offsets::camera_fovy);
	}

	Vector3 calc_angle(Vector3 enemypos, Vector3 camerapos)
	{
		Vector3 dir = enemypos - camerapos;

		float x = asin(dir.z / dir.Length()) * 57.2957795131f;
		float z = atan(dir.y / dir.x) * 57.2957795131f;

		if (dir.x >= 0.f) z += 180.f;
		if (x > 179.99f) x -= 360.f;
		else if (x < -179.99f) x += 360.f;

		return Vector3(x, 0.f, z + 90.f);
	}

	bool damage_multiplier() {
		if (globals::damage) {
			uint64_t damgebase = Read<uint64_t>(base_address + offsets::game_manager); //game M

			if (!damgebase)
				return false;

			uint64_t dmage = Read<uint64_t>(damgebase + 0x1F8);
			uint64_t dmage1 = Read<uint64_t>(dmage + 0xD8);
			uint64_t dmage2 = Read<uint64_t>(dmage1 + 0x48);
			uint64_t dmage3 = Read<uint64_t>(dmage2 + 0x130);
			uint64_t dmage4 = Read<uint64_t>(dmage3 + 0x130);
			uint64_t dmage5 = Read<uint64_t>(dmage4 + 0x0);

			Write<unsigned int>(dmage5 + 0x40, globals::damagemultiplier); //dmg
		}
		return true;
	}

	uint32_t entity_health(uintptr_t entity)
	{
		auto r1 = Read<uintptr_t>(entity + 0x28);
		auto r2 = Read<uintptr_t>(r1 + 0xd8);
		auto r3 = Read<uintptr_t>(r2 + 0x8);
		return Read<uint32_t>(r3 + 0x148);
	}

	Vector4 calculate_quaternion(Vector3 euler)
	{
		Vector4 result{};

		auto yaw = (euler.z * 0.01745329251f) * 0.5f;
		auto sy = std::sin(yaw);
		auto cy = std::cos(yaw);

		auto roll = (euler.x * 0.01745329251f) * 0.5f;
		auto sr = std::sin(roll);
		auto cr = std::cos(roll);

		constexpr auto sp = 0.f;
		constexpr auto cp = 1.f;

		result.x = cy * sr * cp - sy * cr * sp;
		result.y = cy * cr * sp + sy * sr * cp;
		result.z = sy * cr * cp - cy * sr * sp;
		result.w = cy * cr * cp + sy * sr * sp;

		return result;
	}

	std::uintptr_t closest_to_fov(Vector3 source_head, Vector3 entity_head, Vector3 source_angle)
	{
		const auto calc_angle = [](Vector3 src, Vector3 dest)
		{
			auto delta = src - dest;

			auto y = -asin(delta.z / src.Distance(dest)) * 57.2957795131f;
			auto x = (atan2(delta.y, delta.x) * 57.2957795131f) + 90.f;

			if (x > 180.f) x -= 360.f;
			else if (x < -180.f) x += 360.f;

			auto clamp = [](Vector3& angle)
			{
				if (angle.y > 75.f) angle.y = 75.f;
				else if (angle.y < -75.f) angle.y = -75.f;
				if (angle.x < -180.f) angle.x += -360.f;
				else if (angle.x > 180.f) angle.x -= 360.f;

				angle.z = 0.f;
			};

			auto angle = Vector3(x, y, 0.f);
			clamp(angle);

			return angle;
		};

		auto aim_angle = calc_angle(source_head, entity_head);

		auto calc_fov = [](Vector3 src, Vector3 aim)
		{
			aim -= src;

			if (aim.x > 180.f)
				aim.x -= 360.f;
			else if (aim.x < -180.f)
				aim.x += 360.f;
			if (aim.y > 180.f)
				aim.y -= 360.f;
			else if (aim.y < -180.f)
				aim.y += 360.f;

			return aim;
		};

		auto angle = calc_fov(source_angle, aim_angle);

		angle.x = std::abs(angle.x);
		angle.y = std::abs(angle.y);

		return angle.x + angle.y;
	}

	Vector3 calculate_euler(Vector4 quat)
	{
		auto y_p2 = quat.y * quat.y;

		auto x = std::atan2(2.f * (quat.w * quat.z + quat.x * quat.y), (1.f - 2.f * (y_p2 + quat.z * quat.z))) * 57.2957795131f;
		auto y = std::atan2(2.f * (quat.w * quat.x + quat.y * quat.z), (1.f - 2.f * (quat.x * quat.x + y_p2))) * 57.2957795131f;

		return Vector3(x, y, 0.f);
	}

	void set_viewangle(std::uintptr_t entity, std::uintptr_t offset, Vector4 value)
	{
		auto r1 = Read<uintptr_t>(entity + 0x20);
		auto r2 = Read<uintptr_t>(r1 + 0x1170);

		Write<Vector4>(r2 + offset, value);
	}
};
