#include <stddef.h>

#include "../standard.h"
// Variables globales

static frame_t * frame;
static frame_t * backFrame;
static uint16_t width=0, height=0;


#define BG_COLOR 0x009999

#define PLAYER1_COLOR 0xAA0000
#define PLAYER2_COLOR 0x00AA00

#define PLAYER_RADIUS 30
#define PLAYER_EYES_RADIUS 10
#define PLAYER_PUPIL_RADIUS 7
#define EYE_SEPARATION_ANGLE 40
//precalculado * 1000 para no usar math.h
//0° apunta a la derecha, incrementa en sentido antihorario
static const int64_t sinVec[360] = {
       0,   17,   35,   52,   70,   87,  105,  122,  139,  156,
     174,  191,  208,  225,  242,  259,  276,  292,  309,  326,
     342,  358,  375,  391,  407,  423,  438,  454,  469,  485,
     500,  515,  530,  545,  559,  574,  588,  602,  616,  629,
     643,  656,  669,  682,  695,  707,  719,  731,  743,  755,
     766,  777,  788,  799,  809,  819,  829,  839,  848,  857,
     866,  875,  883,  891,  899,  906,  914,  921,  927,  934,
     940,  946,  951,  956,  961,  966,  970,  974,  978,  982,
     985,  988,  990,  993,  995,  996,  998,  999,  999, 1000,
    1000, 1000,  999,  999,  998,  996,  995,  993,  990,  988,
     985,  982,  978,  974,  970,  966,  961,  956,  951,  946,
     940,  934,  927,  921,  914,  906,  899,  891,  883,  875,
     866,  857,  848,  839,  829,  819,  809,  799,  788,  777,
     766,  755,  743,  731,  719,  707,  695,  682,  669,  656,
     643,  629,  616,  602,  588,  574,  559,  545,  530,  515,
     500,  485,  469,  454,  438,  423,  407,  391,  375,  358,
     342,  326,  309,  292,  276,  259,  242,  225,  208,  191,
     174,  156,  139,  122,  105,   87,   70,   52,   35,   17,
       0,  -17,  -35,  -52,  -70,  -87, -105, -122, -139, -156,
    -174, -191, -208, -225, -242, -259, -276, -292, -309, -326,
    -342, -358, -375, -391, -407, -423, -438, -454, -469, -485,
    -500, -515, -530, -545, -559, -574, -588, -602, -616, -629,
    -643, -656, -669, -682, -695, -707, -719, -731, -743, -755,
    -766, -777, -788, -799, -809, -819, -829, -839, -848, -857,
    -866, -875, -883, -891, -899, -906, -914, -921, -927, -934,
    -940, -946, -951, -956, -961, -966, -970, -974, -978, -982,
    -985, -988, -990, -993, -995, -996, -998, -999, -999, -1000,
    -1000, -1000, -999, -999, -998, -996, -995, -993, -990, -988,
    -985, -982, -978, -974, -970, -966, -961, -956, -951, -946,
    -940, -934, -927, -921, -914, -906, -899, -891, -883, -875,
    -866, -857, -848, -839, -829, -819, -809, -799, -788, -777,
    -766, -755, -743, -731, -719, -707, -695, -682, -669, -656,
    -643, -629, -616, -602, -588, -574, -559, -545, -530, -515,
    -500, -485, -469, -454, -438, -423, -407, -391, -375, -358,
    -342, -326, -309, -292, -276, -259, -242, -225, -208, -191,
    -174, -156, -139, -122, -105,  -87,  -70,  -52,  -35,  -17
};

// Returns sine * 1000, no floating point needed
static int64_t sin(int64_t deg) {
    while (deg < 0) deg += 360;
    while (deg >= 360) deg -= 360;
    return sinVec[deg];
}

static int64_t cos(int64_t deg) {
    deg += 90;
    while (deg < 0) deg += 360;
    while (deg >= 360) deg -= 360;
    return sinVec[deg];
}

typedef struct PlayerStruct {
    int64_t x;
    int64_t y;
    int64_t rotation;
    uint64_t color;
    int64_t velocity;
} player_t;

typedef struct HoleStruct {
    int64_t x;
    int64_t y;
    uint16_t radius;
    uint64_t color;
} hole_t;

typedef struct ObjectStruct {
    hole_t hole;
    uint8_t isMountain; // 1 if mountain, 0 si es agujero
} object_t;

/* Object instances */
static player_t player1 = {
    .x = 100,
    .y = 100,
    .rotation = 0,
    .color = PLAYER1_COLOR,
    .velocity = 0
};
static player_t player2 = {
    .x = 700,
    .y = 500,
    .rotation = 35,
    .color = PLAYER2_COLOR,
    .velocity = 0
};

static hole_t targetHole = {
    .x = 500,
    .y = 500,
    .radius = 80,
    .color = 0x000000
};

static hole_t ball = {
    .x = 50,
    .y = 350,
    .radius = 20,
    .color = 0xBBBBBB
};
static object_t pit = {
    {
        .x = 200,
        .y = 200,
        .radius = 80,
        .color = 0x005555
    },
    .isMountain = 0
};
static object_t mount = {
    {
        .x = 400,
        .y = 200,
        .radius = 80,
        .color = 0x00AAAA
    },
    .isMountain = 1
};


#define Vel_Step 10 //velocities in pps
#define Vel_Cap 90
#define Rot_Step 5 //deg per 100ms

/*
 * Updates player rotations and velocity when called
 */
// Keyboard makecodes
#define KEY_W 0x11
#define KEY_A 0x1E
#define KEY_S 0x1F
#define KEY_D 0x20
#define KEY_I 0x17
#define KEY_J 0x24
#define KEY_K 0x25
#define KEY_L 0x26

// 2. Fix the movement handling with better scaling
void handleMovements(
    uint64_t deltaTime,
    player_t * player,
    int32_t forwardMakeCode,
    int32_t backMakeCode,
    int32_t leftMakeCode,
    int32_t rightMakeCode) {

    if (isKeyPressed(forwardMakeCode)) {
        if (isKeyPressed(forwardMakeCode)) {
            int64_t cos_val = cos(player->rotation);  // -1000 to 1000
            int64_t base_movement = (Vel_Step * deltaTime) / 100;  // Scale deltaTime down first
            int64_t dx = (cos_val * base_movement) / 1000;
            int64_t dy = (sin(player->rotation) * base_movement) / 1000;

            player->x += dx;
            player->y -= dy;
        }
    }
    if (isKeyPressed(backMakeCode)) {
        int64_t cos_val = cos(player->rotation);  // -1000 to 1000
        int64_t base_movement = (Vel_Step * deltaTime) / 100;  // Scale deltaTime down first
        int64_t dx = (cos_val * base_movement) / 1000;
        int64_t dy = (sin(player->rotation) * base_movement) / 1000;

        player->x -= dx;
        player->y += dy;
    }
    if (isKeyPressed(leftMakeCode)) {
    //    player->rotation += (Rot_Step * deltaTime) / (100 * 1000); // Scale by deltaTime
        player->rotation += Rot_Step; // Scale by deltaTime
   //     while (player->rotation > 360) player->rotation -= 360;
     //   while (player->rotation < 0) player->rotation += 360;
    }
    if (isKeyPressed(rightMakeCode)) {
       // player->rotation -= (Rot_Step * deltaTime) / (100 * 1000); // Scale by deltaTime
        player->rotation -= Rot_Step; // Scale by deltaTime
       // while (player->rotation > 360) player->rotation -= 360;
       // while (player->rotation < 0) player->rotation += 360;
    }
}

typedef struct point {
    int64_t x, y;
} point_t;
static void getPlayerEyePos(player_t p, point_t * Lcenter, point_t * Rcenter) {
    int left_eye_angle = p.rotation - EYE_SEPARATION_ANGLE;
    int right_eye_angle = p.rotation + EYE_SEPARATION_ANGLE;
    Lcenter->x = p.x + PLAYER_RADIUS * cos(left_eye_angle) / 1000;
    Lcenter->y = p.y - PLAYER_RADIUS * sin(left_eye_angle) / 1000; // Signo - para invertir Y
    Rcenter->x = p.x + PLAYER_RADIUS * cos(right_eye_angle) / 1000;
    Rcenter->y = p.y - PLAYER_RADIUS * sin(right_eye_angle) / 1000; // Signo - para invertir Y
}
static void getPlayerPupilPos(player_t p, point_t * Lpupil, point_t * Rpupil) {
    point_t Leye, Reye;
    getPlayerEyePos(p, &Leye, &Reye);

    int left_eye_angle = p.rotation - EYE_SEPARATION_ANGLE / 2;
    int pupil_distance = PLAYER_EYES_RADIUS - PLAYER_PUPIL_RADIUS;
    Lpupil->x = Leye.x + pupil_distance * cos(left_eye_angle) / 1000;
    Lpupil->y = Leye.y - pupil_distance * sin(left_eye_angle) / 1000; // Signo - para invertir Y

    int right_eye_angle = p.rotation + EYE_SEPARATION_ANGLE / 2;
    Rpupil->x = Reye.x + pupil_distance * cos(right_eye_angle) / 1000;
    Rpupil->y = Reye.y - pupil_distance * sin(right_eye_angle) / 1000; // Signo - para invertir Y
}

void clearPlayer(player_t p) {
    frameDrawCircle(frame, BG_COLOR, p.x, p.y, PLAYER_RADIUS);
    point_t Leye, Reye;
    getPlayerEyePos(p, &Leye, &Reye);
    frameDrawCircle(frame, BG_COLOR, Leye.x, Leye.y, PLAYER_EYES_RADIUS);
    frameDrawCircle(frame, BG_COLOR, Reye.x, Reye.y, PLAYER_EYES_RADIUS);
}
void drawPlayer(player_t p) {
    frameDrawCircle(frame, p.color, p.x, p.y, PLAYER_RADIUS);
    point_t Leye, Reye;
    getPlayerEyePos(p, &Leye, &Reye);
    frameDrawCircle(frame, 0xFFFFFF, Leye.x, Leye.y, PLAYER_EYES_RADIUS);
    frameDrawCircle(frame, 0xFFFFFF, Reye.x, Reye.y, PLAYER_EYES_RADIUS);

    point_t Lpupil, Rpupil;
    getPlayerPupilPos(p, &Lpupil, &Rpupil);
    frameDrawCircle(frame, 0x000000, Lpupil.x, Lpupil.y, PLAYER_PUPIL_RADIUS);
    frameDrawCircle(frame, 0x000000, Rpupil.x, Rpupil.y, PLAYER_PUPIL_RADIUS);
}
void updatePlayerPos(point_t* newPos, player_t* player) {
    // Update player position with boundary checking

    // Check X boundaries
    if (newPos->x < 0) {
        player->x = 0;
    } else if (newPos->x >= frame->width) {
        player->x = frame->width - 1;
    } else {
        player->x = newPos->x;
    }

    // Check Y boundaries
    if (newPos->y < 0) {
        player->y = 0;
    } else if (newPos->y >= frame->height) {
        player->y = frame->height - 1;
    } else {
        player->y = newPos->y;
    }
}

/*
 * Mueve los jugadores para resolver las colisiones
 * Retorna 1 si ambos jugadores colisionan, 0 si no
 */
int checkPlayerCollision(player_t *p1, player_t *p2) {
    int64_t dx = p1->x - p2->x;
    int64_t dy = p1->y - p2->y;
    int64_t distanceSquared = dx*dx + dy*dy;

    //Distancia minima para colisionar
    int64_t minDistance = 2 * PLAYER_RADIUS;
    int64_t minDistanceSquared = minDistance * minDistance;

    //Si no colisiono no hago nada mas
    if (distanceSquared >= minDistanceSquared) {
        return 0;
    }

    // Collision detected - resolve overlap
    if (distanceSquared == 0) {
        // Special case: exactly on top of each other
        p1->x += PLAYER_RADIUS;
        p2->x -= PLAYER_RADIUS;
    } else {
        // Calculate actual distance (approximate without sqrt)
        int64_t distance = 0;
        int64_t temp = distanceSquared;

        // Fast integer square root approximation
        int64_t root = 0;
        int64_t bit = 1 << 30; // Second-to-top bit set
        while (bit > temp) bit >>= 2;
        while (bit != 0) {
            if (temp >= root + bit) {
                temp -= root + bit;
                root += bit << 1;
            }
            root >>= 1;
            bit >>= 2;
        }
        distance = root;

        // Calculate overlap depth
        int64_t overlap = minDistance - distance;

        // Normalize direction vector
        int64_t nx = (dx * 1000) / distance; // Fixed-point scaling
        int64_t ny = (dy * 1000) / distance;

        // Move players apart by half the overlap each
        p1->x += (nx * overlap) / 2000; // Divide by 2 and scaling factor
        p1->y += (ny * overlap) / 2000;
        p2->x -= (nx * overlap) / 2000;
        p2->y -= (ny * overlap) / 2000;
    }

    // Ensure players stay within bounds
    if (p1->x < PLAYER_RADIUS) p1->x = PLAYER_RADIUS;
    if (p1->x > frame->width - PLAYER_RADIUS) p1->x = frame->width - PLAYER_RADIUS;
    if (p1->y < PLAYER_RADIUS) p1->y = PLAYER_RADIUS;
    if (p1->y > frame->height - PLAYER_RADIUS) p1->y = frame->height - PLAYER_RADIUS;

    if (p2->x < PLAYER_RADIUS) p2->x = PLAYER_RADIUS;
    if (p2->x > frame->width - PLAYER_RADIUS) p2->x = frame->width - PLAYER_RADIUS;
    if (p2->y < PLAYER_RADIUS) p2->y = PLAYER_RADIUS;
    if (p2->y > frame->height - PLAYER_RADIUS) p2->y = frame->height - PLAYER_RADIUS;

    return 1;
}

/*
 * Retorna 1 si el jugador especificado colisiona con el agujero, 0 si no
 */
int checkBallCollision(player_t player, hole_t *hole) {
    int64_t dx = player.x - hole->x;
    int64_t dy = player.y - hole->y;
    int64_t distanceSquared = dx*dx + dy*dy;

    int64_t minDistance = PLAYER_RADIUS + hole->radius;
    int64_t minDistanceSquared = minDistance * minDistance;

    // if not colliding
    if (distanceSquared >= minDistanceSquared) {
        return 0;
    }

    // collision detected - move ball
    if (distanceSquared == 0) {
        hole->x += hole->radius;
    } else {
        int64_t distance = 0;
        int64_t temp = distanceSquared;

        // Aproximacion sqrt
        int64_t root = 0;
        int64_t bit = 1 << 30;
        while (bit > temp) bit >>= 2;
        while (bit != 0) {
            if (temp >= root + bit) {
                temp -= root + bit;
                root += bit << 1;
            }
            root >>= 1;
            bit >>= 2;
        }
        distance = root;

        // overlap depth
        int64_t overlap = minDistance - distance;

        // Normalizar dircetion vec
        int64_t nx = (dx * 1000) / distance; // Fixed-point scaling
        int64_t ny = (dy * 1000) / distance;

        //Move hole away from player
        hole->x -= (nx * overlap) / 1000;
        hole->y -= (ny * overlap) / 1000;
    }

    // Bounds checking
    if (hole->x < hole->radius) hole->x = hole->radius;
    if (hole->x > frame->width - hole->radius) hole->x = frame->width - hole->radius;
    if (hole->y < hole->radius) hole->y = hole->radius;
    if (hole->y > frame->height - hole->radius) hole->y = frame->height - hole->radius;

    return 1;
}

int checkHoleInclusion(hole_t *outerHole, hole_t *innerHole) {
    // Distancia entre centros
    int64_t dx = outerHole->x - innerHole->x;
    int64_t dy = outerHole->y - innerHole->y;
    int64_t distanceSquared = dx*dx + dy*dy;

    // Distancia máxima permitida entre los agujeros (Tolerancia de radios)
    int64_t maxAllowedDistance = outerHole->radius - innerHole->radius;

    if (maxAllowedDistance < 0) {
        return 0;
    }

    //Comparo modulos
    int64_t maxAllowedDistanceSquared = maxAllowedDistance * maxAllowedDistance;
    return (distanceSquared <= maxAllowedDistanceSquared);
}

int checkIsPlayerInHole(player_t *player, hole_t *hole) {
    hole_t auxHole = {
        .x = player->x,
        .y = player->y,
        .radius = PLAYER_RADIUS,
        .color = 0xFF00FF
    };
    return checkHoleInclusion(hole, &auxHole);
}

void drawHole(hole_t holeToDraw) {
    frameDrawCircle(frame, holeToDraw.color, holeToDraw.x, holeToDraw.y, holeToDraw.radius);
}

uint8_t newFb [FRAMEBUFFER_SIZE];
frame_t newFrame;

uint8_t newBackFb [FRAMEBUFFER_SIZE];
frame_t newBackFrame;

void playSound(int freq, uint64_t ms) {
    startSound(freq);
    uint64_t start = getBootTime();
    while (getBootTime() - start < ms);
    stopSound();
}
void respawnSound() {
    playSound(1000, 1000);
}

static uint64_t lastTick = 0;
int player1Score = 0;
int player2Score = 0;
int ballControl = 0; // 0 = player1, 1 = player2, quien controla la pelota para saber de quien es el punto
int main() {
    frame = &newFrame;
    frameInit(frame, newFb);
    backFrame = &newBackFrame;
    frameInit(backFrame, newBackFb);
    getVideoData(&width,&height,NULL,NULL);
    frameFill(frame, BG_COLOR);
    frameFill(backFrame, BG_COLOR);
    hole_t drawables[] = {ball, *(hole_t*)&pit, *(hole_t*)&mount, targetHole};
    for (int i = 0; i < sizeof(drawables) / sizeof(drawables[0]); i++) {
        frameDrawCircle(backFrame, drawables[i].color, drawables[i].x, drawables[i].y, drawables[i].radius);
        frameDrawCircle(frame, drawables[i].color, drawables[i].x, drawables[i].y, drawables[i].radius);
    }
    lastTick = getBootTime();
    fontmanager_set_font(2);
    while(1) {
        uint64_t currentTime = getBootTime();
        uint64_t deltaTime = currentTime - lastTick;
        lastTick = currentTime;


        clearPlayer(player1);
        clearPlayer(player2);

        handleMovements(deltaTime, &player1, KEY_W, KEY_S, KEY_A, KEY_D);
        handleMovements(deltaTime, &player2, KEY_I, KEY_K, KEY_J, KEY_L);

        checkPlayerCollision(&player1, &player2);

        frameDrawCircle(frame, BG_COLOR, ball.x, ball.y, ball.radius);
        checkBallCollision(player1, &ball);
        checkBallCollision(player2, &ball);

        //Re-spawns
        if (checkIsPlayerInHole(&player1, &targetHole)){
            player1.x = 50;
            player1.y = 50;
            respawnSound();
        }
        if (checkIsPlayerInHole(&player2, &targetHole)){
            player2.x = 300;
            player2.y = 50;
            respawnSound();
        }



        if (checkHoleInclusion(&targetHole, &ball)) {
            if (ballControl == 0) {
                player1Score++;
            } else {
                player2Score++;
            }
            ball.x = 50;
            ball.y = 350;
        }

        int textLen = 15;
        frameDrawText(frame, "Player 1 score:", PLAYER1_COLOR, 0x000000, 10, 10);
        frameDrawInt(frame, player1Score, PLAYER1_COLOR, 0x000000, 10 + textLen * fontmanager_get_current_font().width, 10);
        frameDrawText(frame, "Player 2 score:", PLAYER2_COLOR, 0x000000, width - (textLen + 5) * fontmanager_get_current_font().width, 10);
        frameDrawInt(frame, player1Score, PLAYER2_COLOR, 0x000000,  width - 5 * fontmanager_get_current_font().width, 10);
        drawPlayer(player1);
        drawPlayer(player2);
        frameDrawCircle(frame, ball.color, ball.x, ball.y, ball.radius);

        setFrame(frame);

     //   }
    }
    //for(int i = 10000000 ; i > 0 ; i --);
    return 1;
}