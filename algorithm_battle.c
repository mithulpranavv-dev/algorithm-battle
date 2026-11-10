/*
 * ALGORITHM BATTLE - "Which Data Structure Wins?"
 * Application : Employee ID Search
 * Structures  : Binary Search Tree (BST), AVL Tree, Hash Table (Linear Probing)
 *
 * How comparisons are counted (same rule for all three):
 *   BST / AVL : every node whose ID is compared with the key = 1 comparison
 *   Hashing   : every table slot looked at (probe)           = 1 comparison
 *
 * Compile : gcc algorithm_battle.c -o battle
 * Run     : ./battle            (or  ./battle 1234  to test your own key in the viva)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N          40      /* number of records in the dataset */
#define TABLE_SIZE 53      /* prime number, load factor = 40/53 = 0.75 */

/* ------------------------------------------------------------------ record */
typedef struct {
    int  id;
    char name[20];
    char dept[8];
} Employee;

/* ------------------------------------------------------------------ dataset */
/* 40 unique IDs, in the order employees were hired (IDs issued in ascending order) */
int IDS[N] = {
    1005, 1012, 1020, 1033, 1041, 1058, 1062, 1077, 1085, 1091,
    1104, 1117, 1123, 1139, 1146, 1152, 1168, 1175, 1183, 1199,
    1204, 1216, 1227, 1231, 1249, 1258, 1263, 1274, 1288, 1295,
    1302, 1319, 1326, 1337, 1344, 1351, 1369, 1378, 1385, 1392
};
const char *NAMES[N] = {
    "Aarav","Bhavna","Chitra","Dev","Esha","Farhan","Gayathri","Hari","Isha","Jayanth",
    "Kavya","Lokesh","Meena","Naveen","Oviya","Prakash","Qadir","Rhea","Suresh","Tara",
    "Uday","Vimala","Waseem","Xavier","Yamini","Zubin","Anand","Bala","Charan","Divya",
    "Elan","Fathima","Gokul","Harini","Imran","Janani","Karthik","Lavanya","Mohan","Nandhini"
};
const char *DEPTS[5] = { "CSE", "ECE", "MECH", "HR", "FIN" };

#define S_COUNT 8
#define F_COUNT 8
int SUCCESS_KEYS[S_COUNT] = { 1005, 1146, 1263, 1337, 1392, 1062, 1216, 1319 };
int FAIL_KEYS[F_COUNT]    = { 1000, 1100, 1200, 1300, 1400, 1250, 1010, 1999 };

/* build the Employee record for a given ID (name/dept picked by its position in IDS) */
Employee make_employee(int id) {
    Employee e;
    int i;
    for (i = 0; i < N; i++)
        if (IDS[i] == id) break;
    if (i == N) {                       /* a new hire that is not in the 40-record list */
        e.id = id;
        strcpy(e.name, "NewHire");
        strcpy(e.dept, "CSE");
        return e;
    }
    e.id = id;
    strcpy(e.name, NAMES[i]);
    strcpy(e.dept, DEPTS[i % 5]);
    return e;
}

/* ==========================================================================
 *                          PART 1 : BINARY SEARCH TREE
 * ========================================================================== */
typedef struct BSTNode {
    Employee emp;
    struct BSTNode *left, *right;
} BSTNode;

int bst_cmp;                            /* comparisons made by the last BST operation */

BSTNode *bst_new(Employee e) {
    BSTNode *n = (BSTNode *)malloc(sizeof(BSTNode));
    n->emp = e;
    n->left = n->right = NULL;
    return n;
}

BSTNode *bst_insert(BSTNode *root, Employee e) {
    bst_cmp = 0;
    if (root == NULL) return bst_new(e);
    BSTNode *cur = root;
    while (1) {
        bst_cmp++;
        if (e.id == cur->emp.id) return root;           /* duplicate: ignore */
        if (e.id < cur->emp.id) {
            if (cur->left == NULL)  { cur->left = bst_new(e);  return root; }
            cur = cur->left;
        } else {
            if (cur->right == NULL) { cur->right = bst_new(e); return root; }
            cur = cur->right;
        }
    }
}

BSTNode *bst_search(BSTNode *root, int id) {
    bst_cmp = 0;
    BSTNode *cur = root;
    while (cur != NULL) {
        bst_cmp++;
        if (id == cur->emp.id) return cur;
        cur = (id < cur->emp.id) ? cur->left : cur->right;
    }
    return NULL;
}

int bst_height(BSTNode *n) {
    if (n == NULL) return 0;
    int l = bst_height(n->left), r = bst_height(n->right);
    return 1 + (l > r ? l : r);
}

void bst_inorder(BSTNode *n)   { if (!n) return; bst_inorder(n->left);  printf("%d ", n->emp.id); bst_inorder(n->right); }
void bst_preorder(BSTNode *n)  { if (!n) return; printf("%d ", n->emp.id); bst_preorder(n->left);  bst_preorder(n->right); }
void bst_postorder(BSTNode *n) { if (!n) return; bst_postorder(n->left); bst_postorder(n->right); printf("%d ", n->emp.id); }

void bst_free(BSTNode *n) { if (!n) return; bst_free(n->left); bst_free(n->right); free(n); }

/* ==========================================================================
 *                               PART 2 : AVL TREE
 * ========================================================================== */
typedef struct AVLNode {
    Employee emp;
    struct AVLNode *left, *right;
    int height;
} AVLNode;

int avl_cmp;                            /* comparisons made by the last AVL operation */
int avl_rotations;                      /* total rotations so far */

int avl_h(AVLNode *n) { return n ? n->height : 0; }
int max_int(int a, int b) { return a > b ? a : b; }
void avl_update(AVLNode *n) { n->height = 1 + max_int(avl_h(n->left), avl_h(n->right)); }
int avl_balance(AVLNode *n) { return n ? avl_h(n->left) - avl_h(n->right) : 0; }

AVLNode *avl_new(Employee e) {
    AVLNode *n = (AVLNode *)malloc(sizeof(AVLNode));
    n->emp = e;
    n->left = n->right = NULL;
    n->height = 1;
    return n;
}

AVLNode *rotate_right(AVLNode *y) {
    AVLNode *x = y->left, *t = x->right;
    x->right = y;
    y->left = t;
    avl_update(y); avl_update(x);
    avl_rotations++;
    return x;
}

AVLNode *rotate_left(AVLNode *x) {
    AVLNode *y = x->right, *t = y->left;
    y->left = x;
    x->right = t;
    avl_update(x); avl_update(y);
    avl_rotations++;
    return y;
}

AVLNode *avl_insert_rec(AVLNode *n, Employee e) {
    if (n == NULL) return avl_new(e);
    avl_cmp++;
    if (e.id == n->emp.id) return n;                    /* duplicate: ignore */
    if (e.id < n->emp.id) n->left  = avl_insert_rec(n->left, e);
    else                  n->right = avl_insert_rec(n->right, e);

    avl_update(n);
    int b = avl_balance(n);
    if (b >  1 && e.id < n->left->emp.id)  return rotate_right(n);                          /* LL */
    if (b < -1 && e.id > n->right->emp.id) return rotate_left(n);                           /* RR */
    if (b >  1 && e.id > n->left->emp.id)  { n->left  = rotate_left(n->left);   return rotate_right(n); } /* LR */
    if (b < -1 && e.id < n->right->emp.id) { n->right = rotate_right(n->right); return rotate_left(n);  } /* RL */
    return n;
}

AVLNode *avl_insert(AVLNode *root, Employee e) {
    avl_cmp = 0;
    return avl_insert_rec(root, e);
}

AVLNode *avl_search(AVLNode *root, int id) {
    avl_cmp = 0;
    AVLNode *cur = root;
    while (cur != NULL) {
        avl_cmp++;
        if (id == cur->emp.id) return cur;
        cur = (id < cur->emp.id) ? cur->left : cur->right;
    }
    return NULL;
}

void avl_inorder(AVLNode *n)  { if (!n) return; avl_inorder(n->left);  printf("%d ", n->emp.id); avl_inorder(n->right); }
void avl_preorder(AVLNode *n) { if (!n) return; printf("%d ", n->emp.id); avl_preorder(n->left); avl_preorder(n->right); }
void avl_free(AVLNode *n)     { if (!n) return; avl_free(n->left); avl_free(n->right); free(n); }

/* ==========================================================================
 *                  PART 3 : HASH TABLE WITH LINEAR PROBING
 * ========================================================================== */
typedef struct {
    Employee slot[TABLE_SIZE];
    int used[TABLE_SIZE];               /* 1 = slot occupied, 0 = empty */
} HashTable;

int ht_cmp;                             /* probes made by the last hash operation */
int ht_collisions;                      /* total collisions over all insertions   */
int ht_last_collisions;                 /* collisions in the last insertion       */

int hash_fn(int id) { return id % TABLE_SIZE; }

void ht_init(HashTable *h) {
    memset(h->used, 0, sizeof(h->used));
    ht_collisions = 0;
}

void ht_insert(HashTable *h, Employee e) {
    int i, pos, idx = hash_fn(e.id);
    ht_cmp = 0;
    ht_last_collisions = 0;
    for (i = 0; i < TABLE_SIZE; i++) {
        pos = (idx + i) % TABLE_SIZE;                   /* linear probing: try next slot */
        ht_cmp++;
        if (!h->used[pos]) { h->slot[pos] = e; h->used[pos] = 1; return; }
        if (h->slot[pos].id == e.id) return;            /* duplicate */
        ht_last_collisions++;                           /* slot taken by a different key */
        ht_collisions++;
    }
    printf("Hash table is full!\n");
}

Employee *ht_search(HashTable *h, int id) {
    int i, pos, idx = hash_fn(id);
    ht_cmp = 0;
    for (i = 0; i < TABLE_SIZE; i++) {
        pos = (idx + i) % TABLE_SIZE;
        ht_cmp++;
        if (!h->used[pos]) return NULL;                 /* empty slot => key not present */
        if (h->slot[pos].id == id) return &h->slot[pos];
    }
    return NULL;
}

void ht_print(HashTable *h) {
    int i;
    for (i = 0; i < TABLE_SIZE; i++) {
        if (h->used[i]) {
            printf("  slot %2d -> %d", i, h->slot[i].id);
            if (hash_fn(h->slot[i].id) != i)
                printf("   (home slot %d, displaced)", hash_fn(h->slot[i].id));
            printf("\n");
        }
    }
}

/* ==========================================================================
 *                              THE EXPERIMENT
 * ========================================================================== */
void run_experiment(const char *title, int order[N], int verbose) {
    BSTNode *bst = NULL;
    AVLNode *avl = NULL;
    HashTable ht;
    int i;
    long bst_ins = 0, avl_ins = 0, ht_ins = 0;

    printf("\n==========================================================\n");
    printf(" DATASET ORDER: %s\n", title);
    printf("==========================================================\n");

    ht_init(&ht);
    avl_rotations = 0;

    /* ---- insert all 40 records into all three structures ---- */
    for (i = 0; i < N; i++) {
        Employee e = make_employee(order[i]);
        bst = bst_insert(bst, e);        bst_ins += bst_cmp;
        avl = avl_insert(avl, e);        avl_ins += avl_cmp;
        ht_insert(&ht, e);               ht_ins  += ht_cmp;
    }

    if (verbose) {
        printf("\nBST inorder   (sorted): "); bst_inorder(bst);
        printf("\nBST preorder          : "); bst_preorder(bst);
        printf("\nBST postorder         : "); bst_postorder(bst);
        printf("\n\nAVL inorder   (sorted): "); avl_inorder(avl);
        printf("\nAVL preorder          : "); avl_preorder(avl);
        printf("\n\nHash table (size %d, h(k) = k %% %d, linear probing):\n", TABLE_SIZE, TABLE_SIZE);
        ht_print(&ht);
    }

    /* ---- successful searches ---- */
    int bs = 0, as = 0, hs = 0;
    printf("\nSUCCESSFUL SEARCH (comparisons)\n");
    printf("%-8s %5s %5s %8s\n", "Key", "BST", "AVL", "Hashing");
    for (i = 0; i < S_COUNT; i++) {
        int k = SUCCESS_KEYS[i];
        bst_search(bst, k); avl_search(avl, k); ht_search(&ht, k);
        printf("%-8d %5d %5d %8d\n", k, bst_cmp, avl_cmp, ht_cmp);
        bs += bst_cmp; as += avl_cmp; hs += ht_cmp;
    }
    printf("%-8s %5.2f %5.2f %8.2f\n", "AVERAGE",
           (double)bs / S_COUNT, (double)as / S_COUNT, (double)hs / S_COUNT);

    /* ---- unsuccessful searches ---- */
    int bu = 0, au = 0, hu = 0;
    printf("\nUNSUCCESSFUL SEARCH (comparisons)\n");
    printf("%-8s %5s %5s %8s\n", "Key", "BST", "AVL", "Hashing");
    for (i = 0; i < F_COUNT; i++) {
        int k = FAIL_KEYS[i];
        bst_search(bst, k); avl_search(avl, k); ht_search(&ht, k);
        printf("%-8d %5d %5d %8d\n", k, bst_cmp, avl_cmp, ht_cmp);
        bu += bst_cmp; au += avl_cmp; hu += ht_cmp;
    }
    printf("%-8s %5.2f %5.2f %8.2f\n", "AVERAGE",
           (double)bu / F_COUNT, (double)au / F_COUNT, (double)hu / F_COUNT);

    /* ---- record the numbers for the 40-record dataset BEFORE the extra insert ---- */
    int bst_h = bst_height(bst), avl_hh = avl_h(avl), rot = avl_rotations, coll = ht_collisions;

    /* ---- insertion of one new employee ---- */
    Employee fresh = make_employee(1400);
    bst = bst_insert(bst, fresh); int b1 = bst_cmp;
    avl = avl_insert(avl, fresh); int a1 = avl_cmp;
    ht_insert(&ht, fresh);        int h1 = ht_cmp;
    printf("\nINSERT NEW KEY 1400 -> BST: %d cmp | AVL: %d cmp | Hashing: %d probe(s)\n", b1, a1, h1);

    /* ---- summary ---- */
    printf("\nSUMMARY\n");
    printf("  Total insertion comparisons (40 records): BST=%ld  AVL=%ld  Hashing=%ld\n", bst_ins, avl_ins, ht_ins);
    printf("  Height  : BST=%d  AVL=%d   (ideal minimum for 40 nodes = 6)\n", bst_h, avl_hh);
    printf("  AVL rotations performed: %d\n", rot);
    printf("  Hash collisions during the 40 insertions: %d   (load factor = %.2f)\n", coll, (double)N / TABLE_SIZE);

    bst_free(bst);
    avl_free(avl);
}

/* viva demo: how does each structure handle a NEW search key? */
void viva(int key) {
    BSTNode *bst = NULL;
    AVLNode *avl = NULL;
    HashTable ht;
    int i;
    ht_init(&ht);
    for (i = 0; i < N; i++) {
        Employee e = make_employee(IDS[i]);
        bst = bst_insert(bst, e);
        avl = avl_insert(avl, e);
        ht_insert(&ht, e);
    }
    printf("\n----------------------------------------------------------\n");
    printf(" VIVA DEMO: search for new key %d\n", key);
    printf("----------------------------------------------------------\n");

    BSTNode *r1 = bst_search(bst, key);
    if (r1) printf("BST     : FOUND %s (%s) in %d comparisons\n", r1->emp.name, r1->emp.dept, bst_cmp);
    else    printf("BST     : NOT FOUND in %d comparisons\n", bst_cmp);

    AVLNode *r2 = avl_search(avl, key);
    if (r2) printf("AVL     : FOUND %s (%s) in %d comparisons\n", r2->emp.name, r2->emp.dept, avl_cmp);
    else    printf("AVL     : NOT FOUND in %d comparisons\n", avl_cmp);

    Employee *r3 = ht_search(&ht, key);
    if (r3) printf("Hashing : FOUND %s (%s) in %d probes (home slot %d)\n", r3->name, r3->dept, ht_cmp, hash_fn(key));
    else    printf("Hashing : NOT FOUND in %d probes (home slot %d)\n", ht_cmp, hash_fn(key));

    bst_free(bst);
    avl_free(avl);
}

/* small fixed random-number generator so the "shuffled" run gives the same result on every computer */
unsigned int rng_state = 42;
unsigned int next_rand(void) {
    rng_state = rng_state * 1103515245u + 12345u;
    return (rng_state >> 16) & 0x7fff;
}

int main(int argc, char *argv[]) {
    int i, j, t;
    int shuffled[N];

    /* Experiment 1: hiring order (ascending IDs) */
    run_experiment("HIRING ORDER (ascending, as IDs are issued)", IDS, 1);

    /* Experiment 2: same 40 records in shuffled order */
    memcpy(shuffled, IDS, sizeof(IDS));
    for (i = N - 1; i > 0; i--) {
        j = next_rand() % (i + 1);
        t = shuffled[i]; shuffled[i] = shuffled[j]; shuffled[j] = t;
    }
    run_experiment("RANDOM ORDER (shuffled, fixed seed 42)", shuffled, 0);

    /* Viva demo: key from command line, or 1234 by default */
    viva(argc > 1 ? atoi(argv[1]) : 1234);
    return 0;
}
