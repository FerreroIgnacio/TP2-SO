#include <stddef.h>

#include "../standard.h"
// Variables globales

static uint8_t newFb [FRAMEBUFFER_SIZE];
static frame_t newFrame;

static uint8_t newBackFb [FRAMEBUFFER_SIZE];
static frame_t newBackFrame;

static uint16_t width=0, height=0;

#define SPAWNCOUNT 16
#define MINOBJECT 1 // Mínimo de objetos por nivel
#define MAXOBJECTS 3

#define OBJECT_MAXSIZE 130
#define OBJECT_MINSIZE 50
#define PIT_COLOR 0x005555
#define MOUNTAIN_COLOR 0x00AAAA

#define MINTARGETS 1
#define MAXTARGETS 3
#define TARGET_MAXSIZE 150
#define TARGET_MINSIZE (BALL_MAXSIZE + 5)
#define TARGET_COLOR 0x000000

#define MINBALLS 1
#define MAXBALLS 3
#define BALL_MAXSIZE 40
#define BALL_MINSIZE 10
#define BALL_COLOR 0xCCCCCC


#define BG_COLOR 0x009999

#define PLAYER1_COLOR 0xAA0000
#define PLAYER2_COLOR 0x00AA00

#define PLAYER_RADIUS 30
#define PLAYER_EYES_RADIUS 10
#define PLAYER_PUPIL_RADIUS 7
#define EYE_SEPARATION_ANGLE 40

#define CIRCLE_REDRAW_MARGIN 5

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
    int64_t speedMult; //Speed multiplier 100 default
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

typedef struct point {
    int64_t x, y;
} point_t;

typedef struct LevelStruct {
    object_t  objects[MAXOBJECTS];
    uint64_t objectsCount;
    hole_t targets[MAXOBJECTS];
    uint16_t targetsCount;
    hole_t balls[MAXBALLS];
    uint16_t ballsCount;
    point_t spawnPosition[SPAWNCOUNT];
} level_t;

/* Object instances */
static player_t player1 = {
    .x = 100,
    .y = 100,
    .rotation = 0,
    .color = PLAYER1_COLOR,
    .velocity = 0,
    .speedMult = 100 // Default speed multiplier
};
static player_t player2 = {
    .x = 700,
    .y = 500,
    .rotation = 35,
    .color = PLAYER2_COLOR,
    .velocity = 0,
    .speedMult = 100 // Default speed multiplier
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




#define Vel_Step 25
#define Rot_Step 10

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
    level_t level,
    int32_t forwardMakeCode,
    int32_t backMakeCode,
    int32_t leftMakeCode,
    int32_t rightMakeCode) {

    // Check if player is in any object and adjust speed multiplier
    player->speedMult = 100; // Default speed

    for (int i = 0; i < level.objectsCount; i++) {
        hole_t playerHole = {
            .x = player->x,
            .y = player->y,
            .radius = PLAYER_RADIUS,
            .color = 0xFF00FF
        };

        if (checkHoleInclusion(&level.objects[i].hole, &playerHole)) {
            if (level.objects[i].isMountain) {
                player->speedMult = 180; // Faster on mountains
            } else {
                player->speedMult = 40;  // Slower in pits
            }
            break; // Use the first object found
        }
    }

    if (isKeyPressed(forwardMakeCode)) {
        int64_t cos_val = cos(player->rotation);  // -1000,1000
        int64_t base_movement = (Vel_Step * deltaTime * player->speedMult) / 10000; // Apply speedMult
        int64_t dx = (cos_val * base_movement) / 1000;
        int64_t dy = (sin(player->rotation) * base_movement) / 1000;

        int64_t new_x = player->x + dx;
        int64_t new_y = player->y - dy;

        // Bounds checking - keep player center within screen
        if (new_x >= 0 && new_x < width) {
            player->x = new_x;
        }
        if (new_y >= 0 && new_y < height) {
            player->y = new_y;
        }
    }

    if (isKeyPressed(backMakeCode)) {
        int64_t cos_val = cos(player->rotation);  // -1000,1000
        int64_t base_movement = (Vel_Step * deltaTime * player->speedMult) / 10000; // Apply speedMult
        int64_t dx = (cos_val * base_movement) / 1000;
        int64_t dy = (sin(player->rotation) * base_movement) / 1000;

        int64_t new_x = player->x - dx;
        int64_t new_y = player->y + dy;

        // Bounds checking - keep player center within screen
        if (new_x >= 0 && new_x < width) {
            player->x = new_x;
        }
        if (new_y >= 0 && new_y < height) {
            player->y = new_y;
        }
    }

    if (isKeyPressed(leftMakeCode)) {
        player->rotation += Rot_Step;
        // Normalize rotation to 0-359 range
        if (player->rotation >= 360) {
            player->rotation -= 360;
        }
    }

    if (isKeyPressed(rightMakeCode)) {
        player->rotation -= Rot_Step;
        // Normalize rotation to 0-359 range
        if (player->rotation < 0) {
            player->rotation += 360;
        }
    }
}
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

void clearPlayer(frame_t * frame, player_t p) {
    frameCopyCircle(p.x, p.y,PLAYER_RADIUS + CIRCLE_REDRAW_MARGIN, frame, &newBackFrame);
    point_t Leye, Reye;
    getPlayerEyePos(p, &Leye, &Reye);
    frameCopyCircle(Leye.x,Leye.y, PLAYER_EYES_RADIUS + CIRCLE_REDRAW_MARGIN, frame, &newBackFrame);
    frameCopyCircle(Reye.x,Reye.y, PLAYER_EYES_RADIUS + CIRCLE_REDRAW_MARGIN, frame, &newBackFrame);
}
/*
 * Dibuja el jugador en el frame especificado
 */
void drawPlayer(frame_t * frame, player_t p) {
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
    // Check X boundaries
    if (newPos->x < 0) {
        player->x = 0;
    } else if (newPos->x >= newFrame.width) {
        player->x = newFrame.width - 1;
    } else {
        player->x = newPos->x;
    }

    // Check Y boundaries
    if (newPos->y < 0) {
        player->y = 0;
    } else if (newPos->y >= newFrame.height) {
        player->y = newFrame.height - 1;
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
    if (p1->x > newFrame.width - PLAYER_RADIUS) p1->x = newFrame.width - PLAYER_RADIUS;
    if (p1->y < PLAYER_RADIUS) p1->y = PLAYER_RADIUS;
    if (p1->y > newFrame.height - PLAYER_RADIUS) p1->y = newFrame.height - PLAYER_RADIUS;

    if (p2->x < PLAYER_RADIUS) p2->x = PLAYER_RADIUS;
    if (p2->x > newFrame.width - PLAYER_RADIUS) p2->x = newFrame.width - PLAYER_RADIUS;
    if (p2->y < PLAYER_RADIUS) p2->y = PLAYER_RADIUS;
    if (p2->y > newFrame.height - PLAYER_RADIUS) p2->y = newFrame.height - PLAYER_RADIUS;

    return 1;
}

/*
 * Retorna el número de colisiones detectadas con las pelotas
 * Modifica las posiciones de las pelotas que colisionan
 */
int checkBallCollisions(player_t player, hole_t *balls, int ballCount) {
    int collisions = 0;

    for (int i = 0; i < ballCount; i++) {
        hole_t *ball = &balls[i];

        int64_t dx = player.x - ball->x;
        int64_t dy = player.y - ball->y;
        int64_t distanceSquared = dx*dx + dy*dy;

        int64_t minDistance = PLAYER_RADIUS + ball->radius;
        int64_t minDistanceSquared = minDistance * minDistance;

        // Si no hay colisión, continuar
        if (distanceSquared >= minDistanceSquared) {
            continue;
        }

        // Colisión detectada
        if (distanceSquared == 0) {
            // Caso especial: posiciones idénticas
            ball->x += minDistance;
        } else {
            // Calcular distancia usando aproximación más simple y confiable
            int64_t distance = 0;
            int64_t temp = distanceSquared;

            // Método de Newton simplificado para raíz cuadrada
            int64_t x = temp;
            int64_t y = (x + 1) / 2;
            while (y < x) {
                x = y;
                y = (x + temp / x) / 2;
            }
            distance = x;

            // Calcular overlap
            int64_t overlap = minDistance - distance;

            // Asegurar que hay overlap significativo
            if (overlap <= 0) {
                continue;
            }

            // Vector de dirección normalizado (de jugador hacia pelota)
            // Usar factor de escala más conservador
            const int64_t SCALE = 1000;

            int64_t nx = (-dx * SCALE) / distance;
            int64_t ny = (-dy * SCALE) / distance;

            // Mover la pelota ALEJÁNDOLA del jugador
            ball->x += (nx * overlap) / SCALE;
            ball->y += (ny * overlap) / SCALE;
        }

        // CORRECCIÓN: Límites de pantalla (width/height estaban intercambiados)
        if (ball->x < ball->radius) {
            ball->x = ball->radius;
        }
        if (ball->x > width - ball->radius) {
            ball->x = width - ball->radius;
        }
        if (ball->y < ball->radius) {
            ball->y = ball->radius;
        }
        if (ball->y > height - ball->radius) {
            ball->y = height - ball->radius;
        }

        collisions++;
    }
    return collisions;
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

void drawHole(frame_t * frame, hole_t holeToDraw) {
    frameDrawCircle(frame, holeToDraw.color, holeToDraw.x, holeToDraw.y, holeToDraw.radius);
}
void drawObject(frame_t * frame, object_t objectToDraw) {
    drawHole(frame, objectToDraw.hole);
}



static uint64_t soundEndTime = 0;
void playSound(int freq, uint64_t ms) {
    startSound(freq);
    uint64_t start = getBootTime();
  //  while (getBootTime() - start < ms);
    soundEndTime = start + ms;
}
void respawnSound() {
    playSound(200, 50);
}
void scoreSound() {
    playSound(1000, 20);
}

int checkCircleCollision(int64_t x1, int64_t y1, int64_t r1, int64_t x2, int64_t y2, int64_t r2) {
    int64_t dx = x1 - x2;
    int64_t dy = y1 - y2;
    int64_t distanceSquared = dx * dx + dy * dy;
    int64_t radiusSum = r1 + r2;
    return distanceSquared < (radiusSum * radiusSum);
}
// Función auxiliar para verificar si un spawn está libre de colisiones con targets
int isSpawnSafe(int64_t spawnX, int64_t spawnY, level_t* level) {
    // Verificar colisión con todos los targets
    for (int i = 0; i < level->targetsCount; i++) {
        if (checkCircleCollision(spawnX, spawnY, PLAYER_RADIUS,
                               level->targets[i].x, level->targets[i].y, level->targets[i].radius)) {
            return 0;
                               }
    }

    // Opcionalmente también verificar colisión con objetos peligrosos (pozos)
    for (int i = 0; i < level->objectsCount; i++) {
        if (!level->objects[i].isMountain) { // Si es un pozo
            if (checkCircleCollision(spawnX, spawnY, PLAYER_RADIUS,
                                   level->objects[i].hole.x, level->objects[i].hole.y,
                                   level->objects[i].hole.radius)) {
                return 0;
                                   }
        }
    }

    return 1;
}

level_t generateLevel() {
    level_t level = {0}; // Initialize all to zero

    // Generate objects
    level.objectsCount = (rand() % (MAXOBJECTS - MINOBJECT + 1)) + MINOBJECT;
    for (int i = 0; i < level.objectsCount; i++) {
        int isMountain = rand() % 2;
        level.objects[i] = (object_t){
            .hole = {
                .x = rand() % (width - OBJECT_MAXSIZE) + OBJECT_MINSIZE,
                .y = rand() % (height - OBJECT_MAXSIZE) + OBJECT_MINSIZE,
                .radius = (rand() % (OBJECT_MAXSIZE - OBJECT_MINSIZE)) + OBJECT_MINSIZE,
                .color = isMountain ? MOUNTAIN_COLOR : PIT_COLOR,
            },
            .isMountain = isMountain
        };
    }

    // Generate targets
    level.targetsCount = (rand() % (MAXTARGETS - MINTARGETS + 1)) + MINTARGETS;
    for (int i = 0; i < level.targetsCount; i++) {
        level.targets[i] = (hole_t){
            .x = rand() % (width - TARGET_MAXSIZE) + TARGET_MINSIZE,
            .y = rand() % (height - TARGET_MAXSIZE) + TARGET_MINSIZE,
            .radius = (rand() % (TARGET_MAXSIZE - TARGET_MINSIZE)) + TARGET_MINSIZE,
            .color = TARGET_COLOR
        };
    }

    // Generate balls
    level.ballsCount = (rand() % (MAXBALLS - MINBALLS + 1)) + MINBALLS;
    for (int i = 0; i < level.ballsCount; i++) {
        level.balls[i] = (hole_t){
            .x = rand() % (width - BALL_MAXSIZE) + BALL_MINSIZE,
            .y = rand() % (height - BALL_MAXSIZE) + BALL_MINSIZE,
            .radius = (rand() % (BALL_MAXSIZE - BALL_MINSIZE)) + BALL_MINSIZE,
            .color = BALL_COLOR
        };
    }

    // Generate spawn points
    for (int i = 0; i < SPAWNCOUNT; i++) {
        int validSpawn = 0;

        while (!validSpawn) {
            level.spawnPosition[i].x = rand() % (width - PLAYER_RADIUS * 2) + PLAYER_RADIUS;
            level.spawnPosition[i].y = rand() % (height - PLAYER_RADIUS * 2) + PLAYER_RADIUS;

            validSpawn = 1; // Asumir que es válido

            // Solo verificar que no esté exactamente en el centro de un target
            for (int j = 0; j < level.targetsCount; j++) {
                if (level.spawnPosition[i].x == level.targets[j].x &&
                    level.spawnPosition[i].y == level.targets[j].y) {
                    validSpawn = 0;
                    break;
                    }
            }
        }
    }

    return level;
}


frame_t * drawLevelBackgroundToFrame(frame_t * frame, level_t level, uint64_t backgroundColor) {
    frameFill(frame, backgroundColor);
    for (int i = 0; i < level.objectsCount; i++) {
        drawObject(frame, level.objects[i]);
    }
    for (int i = 0; i < level.targetsCount; i++) {
        drawHole(frame,level.targets[i]);
    }
    return frame;
}
frame_t * drawLevelToFrame(frame_t * frame, level_t level, uint64_t backgroundColor) {
    drawLevelBackgroundToFrame(frame, level, backgroundColor);
    player_t players[2] = {player1, player2};
    for (int i = 0; i < 2; i++) {
        drawPlayer(frame, players[i]);
    }
    for (int i = 0; i < level.ballsCount; i++) {
        drawHole(frame, level.balls[i]);
    }
}

static uint64_t lastTick = 0;
int player1Score = 0;
int player2Score = 0;
int ballControl = 0; // 0 = player1, 1 = player2, quien controla la pelota para saber de quien es el punto, -1 ambos ganan los dos
int runPongisGolf() {
    frameInit(&newFrame, newFb);
    frameInit(&newBackFrame, newBackFb);
    getVideoData(&width,&height,NULL,NULL);

    lastTick = getBootTime();
    fontmanager_set_font(2);
    level_t startLevel = generateLevel();
    player1.x = startLevel.spawnPosition[0].x;
    player1.y = startLevel.spawnPosition[0].y;

    player2.x = startLevel.spawnPosition[1].x;
    player2.y = startLevel.spawnPosition[1].y;

    drawLevelBackgroundToFrame(&newBackFrame, startLevel, BG_COLOR);
    drawLevelToFrame(&newFrame, startLevel, BG_COLOR);

    while(1) {
        if (isKeyPressed(0x3C)) return 1;
        uint64_t currentTime = getBootTime();
        uint64_t deltaTime = currentTime - lastTick;
        lastTick = currentTime;


        clearPlayer(&newFrame, player1);
        clearPlayer(&newFrame, player2);

        handleMovements(deltaTime, &player1, startLevel, KEY_W, KEY_S, KEY_A, KEY_D);
        handleMovements(deltaTime, &player2, startLevel, KEY_I, KEY_K, KEY_J, KEY_L);

        checkPlayerCollision(&player1, &player2);

        for (int i = 0; i < startLevel.ballsCount; i++) {
            hole_t ball = startLevel.balls[i];
            frameCopyCircle(ball.x, ball.y, ball.radius + CIRCLE_REDRAW_MARGIN, &newFrame, &newBackFrame);
        }
        int collisions1 = checkBallCollisions(player1, startLevel.balls, startLevel.ballsCount);
        int collisions2 = checkBallCollisions(player2, startLevel.balls, startLevel.ballsCount);

        // Actualizar quién controla la pelota
        if (collisions1 > 0 && collisions2 > 0) {
            ballControl = -1; // Indica que ambos jugadores tocaron la pelota
        }
        else if (collisions1 > 0) {
            ballControl = 0; // Player 1 tocó la pelota
        }
        else if (collisions2 > 0) {
            ballControl = 1; // Player 2 tocó la pelota
        }
        //Re-spawns
        for (int i = 0; i < startLevel.targetsCount; i++) {
            hole_t aux = startLevel.targets[i];
            if (checkIsPlayerInHole(&player1, &aux)){
                player1.x = startLevel.spawnPosition[0].x;
                player1.y = startLevel.spawnPosition[0].y;
                respawnSound();
            }
            if (checkIsPlayerInHole(&player2, &aux)){
                player2.x = startLevel.spawnPosition[1].x;
                player2.y = startLevel.spawnPosition[1].y;
                respawnSound();
            }
        }
        for (int i = 0; i < startLevel.targetsCount; i++) {
            for (int j = 0; j < startLevel.ballsCount; j++) {
                // Verificar si la pelota está dentro del target
                if (checkHoleInclusion(&startLevel.targets[i], &startLevel.balls[j])) {
                    // ¡GOL! Sumar punto al jugador que controló la pelota
                    if (ballControl == -1) {
                        player1Score++;
                        player2Score++;
                    }
                    else if (ballControl == 0) {
                        player1Score++;
                    } else {
                        player2Score++;
                    }

                    // Respawn de la pelota en posición aleatoria
                    startLevel.balls[j].x = rand() % (width - startLevel.balls[j].radius * 2) + startLevel.balls[j].radius;
                    startLevel.balls[j].y = rand() % (height - startLevel.balls[j].radius * 2) + startLevel.balls[j].radius;

                    // Opcional: sonido de gol
                    scoreSound();
                }
            }
        }


        int textLen = 15;
        drawPlayer(&newFrame, player1);
        drawPlayer(&newFrame, player2);

        for (int i = 0; i < startLevel.ballsCount; i++) {
            hole_t ball = startLevel.balls[i];
            drawHole(&newFrame, ball);
        }

        frameDrawText(&newFrame, "Player 1 score:", PLAYER1_COLOR, 0x000000, 10, 10);
        frameDrawInt(&newFrame, player1Score, PLAYER1_COLOR, 0x000000, 10 + textLen * fontmanager_get_current_font().width, 10);
        frameDrawText(&newFrame, "Player 2 score:", PLAYER2_COLOR, 0x000000, width - (textLen + 5) * fontmanager_get_current_font().width, 10);
        frameDrawInt(&newFrame, player2Score, PLAYER2_COLOR, 0x000000,  width - 5 * fontmanager_get_current_font().width, 10);
       // frameDrawInt(&newFrame, s, 0xFF0000, 000000, 0, 0 );
        //frameDrawCircle(&newFrame, ball.color, ball.x, ball.y, ball.radius);

        setFrame(&newFrame);
        if (soundEndTime != 0 && soundEndTime < getBootTime()) {
            stopSound();
            soundEndTime = 0;
        }

     //   }
    }
    //for(int i = 10000000 ; i > 0 ; i --);
    return 1;
}