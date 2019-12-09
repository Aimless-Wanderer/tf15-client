#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"
#include "tf_defs.h"

LINK_ENTITY_TO_CLASS( tf_weapon_axe, CTFAxe )

void CTFAxe::Spawn()
{
    Precache();
    m_iId = 5;
    m_iClip = -1;
    pev->solid = SOLID_TRIGGER;
}

void CTFAxe::Precache()
{
    PRECACHE_MODEL("models/v_umbrella.mdl");
    PRECACHE_MODEL("models/p_umbrella.mdl");
    PRECACHE_MODEL("models/v_tfc_crowbar.mdl");
    PRECACHE_MODEL("models/p_crowbar.mdl");
    PRECACHE_MODEL("models/p_crowbar2.mdl");
    PRECACHE_SOUND("weapons/cbar_hit1.wav");
    PRECACHE_SOUND("weapons/cbar_hit2.wav");
    PRECACHE_SOUND("weapons/cbar_hitbod1.wav");
    PRECACHE_SOUND("weapons/cbar_hitbod2.wav");
    PRECACHE_SOUND("weapons/cbar_hitbod3.wav");
    PRECACHE_SOUND("weapons/cbar_miss1.wav");
    classid = PC_SOLDIER;
    m_usAxe = PRECACHE_EVENT(1, "events/wpn/tf_axe.sc");
    m_usAxeDecal = PRECACHE_EVENT(1, "events/wpn/tf_axedecal.sc");
}

int CTFAxe::GetItemInfo( ItemInfo *p )
{
    p->pszAmmo1 = 0;
    p->iMaxAmmo1 = -1;
    p->pszName = STRING( pev->classname );
    p->pszAmmo2 = 0;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = -1;
    p->iSlot = 0;
    p->iPosition = 3;
    p->iFlags = 0;
    p->iId = WEAPON_AXE;
    p->iWeight = 0;
    return 1;
}

void CTFAxe::Holster()
{
    m_pPlayer->m_flNextAttack = 0.5;
    SendWeaponAnim(CROWBAR_HOLSTER, 1);
}

BOOL CTFAxe::AxeHit(CBaseEntity *pTarget, Vector p_vecDir, TraceResult *ptr)
{
    ClearMultiDamage();
    if(m_pPlayer->pev->playerclass != PC_SPY)
    {
        pTarget->TraceAttack(m_pPlayer->pev, 20, p_vecDir, ptr, 128);
        ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);
        return 1;
    }

    if(pTarget->IsPlayer())
    {
        UTIL_MakeVectors(pTarget->pev->v_angle);
        UTIL_MakeVectors(m_pPlayer->pev->v_angle);
        if(gpGlobals->v_right.x * gpGlobals->v_forward.y - gpGlobals->v_right.y * gpGlobals->v_forward.x <= 0.0)
        {
            pTarget->TraceAttack(m_pPlayer->pev, 40, p_vecDir, ptr, 128);
        }
        else
        {
            if(m_bHullHit)
            {
                pTarget->TraceAttack(m_pPlayer->pev, 120, p_vecDir, ptr, 128);
            }
            else
            {
                pTarget->TraceAttack(m_pPlayer->pev, 60, p_vecDir, ptr, 128);
            }
        }
    }
    else
    {
        pTarget->TraceAttack(m_pPlayer->pev, 20, p_vecDir, ptr, 128);
        ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);
        return 1;
    }

    ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);
    return 1;
}

BOOL CTFAxe::Deploy()
{
	if(m_pPlayer->pev->playerclass == PC_CIVILIAN)
	    return DefaultDeploy( "models/v_umbrella.mdl", "models/p_umbrella.mdl", CROWBAR_DRAW, "crowbar", 1 );
    else
	    return DefaultDeploy( "models/v_tfc_crowbar.mdl", "models/p_crowbar.mdl", CROWBAR_DRAW, "crowbar", 1 );
}

void CTFAxe::PrimaryAttack()
{
    Vector vecSrc, vecEnd;
    TraceResult tr;
    
    m_bHullHit = 0;
    UTIL_MakeVectors(m_pPlayer->pev->v_angle);
    vecSrc = m_pPlayer->GetGunPosition();
    vecEnd = gpGlobals->v_forward * 32.0f + vecSrc;
    UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, ENT(m_pPlayer->pev), &tr);
    PLAYBACK_EVENT_FULL(1, ENT(m_pPlayer->pev), m_usAxe, 0, (float *)&g_vecZero, (float *)&g_vecZero, 0, 0, classid, 0, 0, 0);
    if ( tr.flFraction >= 1.0 )
    {
        m_bHullHit = 1;
        m_flNextPrimaryAttack = 1.0;
        m_flTimeWeaponIdle = 5;
        m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
        return;
    }
    m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
    m_flTimeWeaponIdle = 5;
    m_flNextPrimaryAttack = 1.0;

    if(!tr.pHit)
    {
        m_pPlayer->m_iWeaponVolume = 128;
    }
    else
    {
        m_pPlayer->m_iWeaponVolume = 512;
    }
}

LINK_ENTITY_TO_CLASS( tf_weapon_spanner, CTFSpanner )

void CTFSpanner::Spawn()
{
    Precache();

    m_iId = 4;
    SET_MODEL(ENT(pev), "models/w_egon.mdl");
    m_iDefaultAmmo = 50;
    pev->solid = SOLID_TRIGGER;
    classid = 1;
}

void CTFSpanner::Precache()
{
    PRECACHE_MODEL("models/v_tfc_spanner.mdl");
    PRECACHE_MODEL("models/p_spanner.mdl");
    PRECACHE_MODEL("models/p_spanner2.mdl");
    m_usAxe = PRECACHE_EVENT(1, "events/wpn/tf_axe.sc");
    m_usAxeDecal = PRECACHE_EVENT(1, "events/wpn/tf_axedecal.sc");
}

void CTFSpanner::Holster()
{
    SendWeaponAnim(5, 1);
    m_pPlayer->m_flNextAttack = 0.5;
}

int CTFSpanner::GetItemInfo( ItemInfo *p )
{
    p->pszAmmo1 = "uranium";
    p->pszName = STRING( pev->classname );
    p->iMaxAmmo1 = -1;
    p->pszAmmo2 = 0;
    p->iMaxAmmo2 = -1;
    p->iSlot = 0;
    p->iPosition = 2;
    p->iFlags = 0;
    p->iMaxClip = -1;
    p->iId = 4;
    p->iWeight = 0;
    return 1;
}

BOOL CTFSpanner::Deploy()
{
	return DefaultDeploy( "models/v_tfc_spanner.mdl", "models/p_spanner.mdl", 4, "crowbar", 1 );
}

void CTFSpanner::WeaponIdle( void )
{
    ResetEmptySound();

    if(m_flTimeWeaponIdle <= 0.0)
    {
        m_flTimeWeaponIdle = 12.5;
        SendWeaponAnim(0, 1);
    }
}
