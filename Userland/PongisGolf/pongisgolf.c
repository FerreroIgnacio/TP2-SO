#include "../standard.h"
// Variables globales

static frame_t * frame;
static frame_t * backFrame;


uint16_t width=0, height=0, bpp=0, pitch=0;


#define BG_COLOR 0x009999

#define PLAYER1_COLOR 0xAA0000
#define PLAYER2_COLOR 0x00AA00

#define PLAYER_RADIUS 30
#define PLAYER_EYES_RADIUS 10
#define PLAYER_PUPIL_RADIUS 7
#define EYE_SEPARATION_ANGLE 40

//precalculado * 1000 para no usar math.h
static const int sinVec[360] = {
    0, 17, 35, 52, 70, 87, 105, 122, 139, 156,
    174, 191, 208, 225, 242, 259, 276, 292, 309, 326,
    342, 358, 375, 391, 407, 423, 438, 454, 469, 485,
    500, 515, 530, 545, 559, 574, 588, 602, 616, 629,
    643, 656, 669, 682, 695, 707, 719, 731, 743, 755,
    766, 777, 788, 799, 809, 819, 829, 839, 848, 857,
    866, 875, 883, 891, 899, 906, 914, 921, 927, 934,
    940, 946, 951, 956, 961, 966, 970, 974, 978, 982,
    985, 988, 990, 993, 995, 996, 998, 999, 999, 1000,
    1000, 1000, 999, 999, 998, 996, 995, 993, 990, 988,
    985, 982, 978, 974, 970, 966, 961, 956, 951, 946,
    940, 934, 927, 921, 914, 906, 899, 891, 883, 875,
    866, 857, 848, 839, 829, 819, 809, 799, 788, 777,
    766, 755, 743, 731, 719, 707, 695, 682, 669, 656,
    643, 629, 616, 602, 588, 574, 559, 545, 530, 515,
    500, 485, 469, 454, 438, 423, 407, 391, 375, 358,
    342, 326, 309, 292, 276, 259, 242, 225, 208, 191,
    174, 156, 139, 122, 105, 87, 70, 52, 35, 17,
    0, -17, -35, -52, -70, -87, -105, -122, -139, -156,
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
    -174, -156, -139, -122, -105, -87, -70, -52, -35, -17
};

//devuelve seno * 1000, no se puede usar floating point SSE no init
static int sin(int deg) {
    while (deg < 0) deg += 360;
    while (deg >= 360) deg -= 360;
    return sinVec[deg];
}

static int cos(int deg) {
    return sin(deg + 90);
}

typedef struct PlayerStruct {
    int64_t x;
    int64_t y;
    int64_t rotation;
    uint64_t color;
    int64_t velocity;
} player_t;

static player_t player1;
static player_t player2;

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


#define Vel_Step 5
#define Vel_Cap 100
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

void handleMovements() {
    // Player 1 controls (WASD)
    if (isKeyPressed(KEY_W)) {
        if (player1.velocity < Vel_Cap) {
            player1.velocity += Vel_Step;
        }
    }
    if (isKeyPressed(KEY_S)) {
        if (player1.velocity > -Vel_Cap) {
            player1.velocity -= Vel_Step;
        }
    }
    if (isKeyPressed(KEY_A)) {
        player1.rotation += Rot_Step;
        while (player1.rotation >= 360) {
            player1.rotation -= 360;
        }
        while (player1.rotation < 0) {
            player1.rotation += 360;
        }
    }
    if (isKeyPressed(KEY_D)) {
        player1.rotation -= Rot_Step;
        while (player1.rotation >= 360) {
            player1.rotation -= 360;
        }
        while (player1.rotation < 0) {
            player1.rotation += 360;
        }
    }

    // Player 2 controls (IJKL)
    if (isKeyPressed(KEY_I)) {
        if (player2.velocity < Vel_Cap) {
            player2.velocity += Vel_Step;
        }
    }
    if (isKeyPressed(KEY_K)) {
        if (player2.velocity > -Vel_Cap) {
            player2.velocity -= Vel_Step;
        }
    }
    if (isKeyPressed(KEY_J)) {
        player2.rotation += Rot_Step;
        while (player2.rotation >= 360) {
            player2.rotation -= 360;
        }
        while (player2.rotation < 0) {
            player2.rotation += 360;
        }
    }
    if (isKeyPressed(KEY_L)) {
        player2.rotation -= Rot_Step;
        while (player2.rotation >= 360) {
            player2.rotation -= 360;
        }
        while (player2.rotation < 0) {
            player2.rotation += 360;
        }
    }
}

typedef struct point {
    int64_t x, y;
} point_t;
void getPlayerEyePos(player_t p, point_t * Lcenter, point_t * Rcenter) {
    int left_eye_angle = p.rotation - EYE_SEPARATION_ANGLE;
    int right_eye_angle = p.rotation + EYE_SEPARATION_ANGLE;
    Lcenter->x = p.x + PLAYER_RADIUS * cos(left_eye_angle) / 1000;
    Lcenter->y = p.y - PLAYER_RADIUS * sin(left_eye_angle) / 1000; // Signo - para invertir Y
    Rcenter->x = p.x + PLAYER_RADIUS * cos(right_eye_angle) / 1000;
    Rcenter->y = p.y - PLAYER_RADIUS * sin(right_eye_angle) / 1000; // Signo - para invertir Y
}

void getPlayerPupilPos(player_t p, point_t * Lpupil, point_t * Rpupil) {
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

uint64_t lastPosUpdate = 0;
void handleInput() {
    //update player rotations and velocities
    handleMovements();


    uint64_t currentTime = getBootTime();
    uint64_t deltaTime = currentTime - lastPosUpdate;
    lastPosUpdate = currentTime;

    point_t newP1, newP2;
    // Convert velocity and time to position delta
    // velocity is already scaled by 1000, sin/cos return values * 1000
    // So we need to divide by 1000000 to get actual pixel movement
    newP1.x = player1.x + (cos(player1.rotation) * player1.velocity * deltaTime) / 1000000;
   // newP1.y = player1.y + (sin(player1.rotation) * player1.velocity * deltaTime) / 1000000;
    newP1.y = player1.y - (sin(player1.rotation) * player1.velocity * deltaTime) / 1000000; // Nota el signo -

    newP2.x = player2.x + (cos(player2.rotation) * player2.velocity * deltaTime) / 1000000;
    newP2.y = player2.y - (sin(player2.rotation) * player2.velocity * deltaTime) / 1000000;

   clearPlayer(player1);
   clearPlayer(player2);

    // Validar límites de pantalla

    // Validar límites antes de asignar nuevas posiciones
   // if (newP1.x >= PLAYER_RADIUS && newP1.x <= width - PLAYER_RADIUS) {
        player1.x = newP1.x;
  //  }
  //  if (newP1.y >= PLAYER_RADIUS && newP1.y <= height - PLAYER_RADIUS) {
        player1.y = newP1.y;
 //   }
    // Validar límites para Player 2
   // if (newP2.x >= PLAYER_RADIUS && newP2.x <= width - PLAYER_RADIUS) {
        player2.x = newP2.x;
   // }
  //  if (newP2.y >= PLAYER_RADIUS && newP2.y <= height - PLAYER_RADIUS) {
        player2.y = newP2.y;
   // }
    drawPlayer(player1);
    drawPlayer(player2);
}

void drawHole(hole_t holeToDraw) {
    frameDrawCircle(frame, holeToDraw.color, holeToDraw.x, holeToDraw.y, holeToDraw.radius);
}
#define FRAMES_PER_SECOND 144




uint8_t newFb [FRAMEBUFFER_SIZE];
frame_t newFrame;

uint8_t newBackFb [FRAMEBUFFER_SIZE];
frame_t newBackFrame;
        
int main() {

    frame = &newFrame;
    frameInit(frame, newFb);

    backFrame = &newBackFrame;
    frameInit(backFrame, newBackFb);

    getVideoData(&width,&height,&bpp,&pitch);
    
    frameFill(frame, BG_COLOR);
    hole_t target = {
        .x = 50,
        .y = 350,
        .radius = 20,
        .color = 0x0A0A0A
    };
    object_t pit = {
        {
            .x = 200,
            .y = 200,
            .radius = 80,
            .color = 0x005555
        },
        .isMountain = 0
    };
    object_t mount = {
         {
            .x = 400,
            .y = 200,
            .radius = 80,
            .color = 0x00AAAA
        },
        .isMountain = 1
    };


    player_t p1 = {
        .x = 100,
        .y = 100,
        .rotation = 0,
        .color = PLAYER1_COLOR,
        .velocity = 0
    };
    player_t p2 = {
        .x = 500,
        .y = 500,
        .rotation = 35,
        .color = PLAYER2_COLOR,
        .velocity = 0
    };
    player1 = p1;
    player2 = p2;

    frameFill(backFrame, BG_COLOR);
    hole_t drawables[] = {target, *(hole_t*)&pit, *(hole_t*)&mount};
    for (int i = 0; i < sizeof(drawables) / sizeof(drawables[0]); i++) {
        frameDrawCircle(backFrame, drawables[i].color, drawables[i].x, drawables[i].y, drawables[i].radius);
        frameDrawCircle(frame, drawables[i].color, drawables[i].x, drawables[i].y, drawables[i].radius);
    }
    uint64_t lastTick = 0;
    drawPlayer(p1);
    drawPlayer(p2);
    lastPosUpdate = getBootTime();
    while(1) {
      //  if (getBootTime() - lastTick > 1000 / FRAMES_PER_SECOND) {
            handleInput();
            setFrame(frame);
            lastTick = getBootTime();
     //   }
    }
    //for(int i = 10000000 ; i > 0 ; i --);
    return 1;
}