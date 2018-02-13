#ifndef COMMON_PHYS_CONSTS_H
#define COMMON_PHYS_CONSTS_H


#define STEP_SIZE_MS            (1000.0 / 120)
#define MAX_STEPS_PER_FRAME     5


// How many meters would be the equivalent of 1 in-game unit?
#define METERS_PER_UNIT         1.0
#define UNITS_PER_METER         (1.0 / METERS_PER_UNIT)

#define MS_PER_SEC              1000.0
#define SEC_PER_MS              (1.0 / MS_PER_SEC)

#define SEC_PER_STEP            (STEP_SIZE_MS * SEC_PER_MS)

#define MPS_TO_UNITS_PER_STEP   (UNITS_PER_METER * STEP_SIZE_MS * SEC_PER_MS)

// m / s / s * (mps_to unit / step) = unit / (step * sec)
// unit / (step * sec) * sec_per_step = unit / (step * step)
#define MPSPS_TO_UNIT_PER_STEP_PER_STEP (MPS_TO_UNITS_PER_STEP * SEC_PER_STEP)


#define RENDER_NEAR_DIST_M      0.1   // m
#define RENDER_FAR_DIST_M       100.0 // m


#define GRAVITY_MODEL_G_MPSPS   (-10.0)   // m/s/s
#define GRAVITY_MODEL_MIN_V_MPS (-40.0)   // m/s  
#define GRAVITY_MODEL_MAX_V_MPS (999.9)   // m/s


#define PLAYER_HITBOX_W         0.5
#define PLAYER_HITBOX_H         2.0
#define PLAYER_HITBOX_D         0.5
#define EYE_VERT_OFFSET         0.9;  // Camera offset above center of player hitbox


#define JUMP_VELOCITY_MPS       4.5  // m/s
#define MOVEMENT_VEL_MPS        2.5  // m/s
#define SPRINT_BOOST            1.7  // multiplication factor

#define PHYS_CONST_PI           3.14159
#define TURN_RATE_RAD_PS        (1.0 * PHYS_CONST_PI) // Turning rate in radians per sec

#define MAX_PITCH_RADS          (0.9 * PHYS_CONST_PI / 2)
#define MIN_PITCH_RADS          (-0.9 * PHYS_CONST_PI / 2)

#endif