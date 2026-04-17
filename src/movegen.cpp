#include "movegen.h"
#include "board.h"

using namespace std;

bool isWhite(Piece p) { return p >= WP && p <= WK; }
bool inBounds(int r, int c) { return r >= 0 && r < 8 && c >= 0 && c < 8; }


bool squareAttacked(int r, int c, bool byWhite) {
    int pDir = byWhite ? 1 : -1;
    if (inBounds(r+pDir, c-1) && board[r+pDir][c-1] == (byWhite ? WP : BP)) return true;
    if (inBounds(r+pDir, c+1) && board[r+pDir][c+1] == (byWhite ? WP : BP)) return true;

    int drN[]={-2,-2,-1,-1,1,1,2,2}, dcN[]={-1,1,-2,2,-2,2,-1,1};
    for(int i=0; i<8; i++) {
        int nr=r+drN[i], nc=c+dcN[i];
        if(inBounds(nr,nc) && board[nr][nc] == (byWhite ? WN : BN)) return true;
    }

    pair<int,int> dirs[]={{-1,0},{1,0},{0,-1},{0,1},{-1,-1},{-1,1},{1,-1},{1,1}};
    for(int i=0; i<8; i++) {
        int nr=r+dirs[i].first, nc=c+dirs[i].second;
        while(inBounds(nr,nc)) {
            Piece p = board[nr][nc];
            if (p != EMPTY) {
                if (i < 4) { if(p==(byWhite?WR:BR) || p==(byWhite?WQ:BQ)) return true; }
                else { if(p==(byWhite?WB:BB) || p==(byWhite?WQ:BQ)) return true; }
                break;
            }
            nr+=dirs[i].first; nc+=dirs[i].second;
        }
    }
    int drK[]={-1,-1,-1,0,0,1,1,1}, dcK[]={-1,0,1,-1,1,-1,0,1};
    for(int i=0; i<8; i++) {
        int nr=r+drK[i], nc=c+dcK[i];
        if(inBounds(nr,nc) && board[nr][nc] == (byWhite ? WK : BK)) return true;
    }
    return false;
}

bool inCheck(bool white) {
    for(int r=0; r<8; r++)
        for(int c=0; c<8; c++)
            if(board[r][c] == (white ? WK : BK)) return squareAttacked(r,c,!white);
    return true;
}

void genSliding(int r, int c, vector<Move>& m, vector<pair<int,int>> d) {
    for(auto& dr : d) {
        int nr=r+dr.first, nc=c+dr.second;
        while(inBounds(nr,nc)) {
            if(board[nr][nc] == EMPTY) {
                m.push_back({r,c,nr,nc,0});
            } else {
                if(isWhite(board[nr][nc]) != whiteToMove) m.push_back({r,c,nr,nc,0});
                break;
            }
            nr+=dr.first; nc+=dr.second;
        }
    }
}


vector<Move> generateMoves() {
    vector<Move> m;
    for(int r=0; r<8; r++) {
        for(int c=0; c<8; c++) {
            Piece p = board[r][c];
            if(p == EMPTY || isWhite(p) != whiteToMove) continue;

           if (p == WP || p == BP) {
    int dir = whiteToMove ? -1 : 1;
    int promoRow = whiteToMove ? 0 : 7;

    int nextR = r + dir;
    if (inBounds(nextR, c) && board[nextR][c] == EMPTY) {
        if (nextR == promoRow) {
            m.push_back(Move(r, c, nextR, c, 1, whiteToMove ? WQ : BQ));
            m.push_back(Move(r, c, nextR, c, 1, whiteToMove ? WR : BR));
            m.push_back(Move(r, c, nextR, c, 1, whiteToMove ? WB : BB));
            m.push_back(Move(r, c, nextR, c, 1, whiteToMove ? WN : BN));

        } else {
            m.push_back(Move(r, c, nextR, c, 0));
            bool startRow = (whiteToMove && r == 6) || (!whiteToMove && r == 1);
            if (startRow && inBounds(r + 2 * dir, c) && board[r + 2 * dir][c] == EMPTY)
                m.push_back(Move(r, c, r + 2 * dir, c, 0));
        }
    }

    for (int dc : {-1, 1}) {
        int nc = c + dc;
        if (inBounds(nextR, nc)) {
            if (board[nextR][nc] != EMPTY && isWhite(board[nextR][nc]) != whiteToMove) {
                if (nextR == promoRow) {
                    m.push_back(Move(r, c, nextR, nc, 1, whiteToMove ? WQ : BQ));
                    m.push_back(Move(r, c, nextR, nc, 1, whiteToMove ? WR : BR));
                    m.push_back(Move(r, c, nextR, nc, 1, whiteToMove ? WB : BB));
                    m.push_back(Move(r, c, nextR, nc, 1, whiteToMove ? WN : BN));
                } else {
                    m.push_back(Move(r, c, nextR, nc, 0));
                }
            }
            if (r == (whiteToMove ? 3 : 4) && nc == enPassantCol) {
                m.push_back(Move(r, c, nextR, nc, 2));
            }
        }
    }
}


              else if(p == WN || p == BN) {
                int dr[]={-2,-2,-1,-1,1,1,2,2}, dc[]={-1,1,-2,2,-2,2,-1,1};
                for(int i=0; i<8; i++)
                    if(inBounds(r+dr[i],c+dc[i]) && (board[r+dr[i]][c+dc[i]]==EMPTY || isWhite(board[r+dr[i]][c+dc[i]])!=whiteToMove))
                        m.push_back({r,c,r+dr[i],c+dc[i],0});
            }
            else if(p == WB || p == BB) genSliding(r,c,m,{{-1,-1},{-1,1},{1,-1},{1,1}});
            else if(p == WR || p == BR) genSliding(r,c,m,{{-1,0},{1,0},{0,-1},{0,1}});
            else if(p == WQ || p == BQ) genSliding(r,c,m,{{-1,-1},{-1,1},{1,-1},{1,1},{-1,0},{1,0},{0,-1},{0,1}});
            else if(p == WK || p == BK) {
                for(int dr=-1; dr<=1; dr++)
                    for(int dc=-1; dc<=1; dc++)
                        if((dr||dc) && inBounds(r+dr,c+dc) && (board[r+dr][c+dc]==EMPTY || isWhite(board[r+dr][c+dc])!=whiteToMove))
                            m.push_back({r,c,r+dr,c+dc,0});

                int s = whiteToMove ? 0 : 1;

                if (!kingMoved[s] && !inCheck(whiteToMove)) {
                    if(!rookRMoved[s] && board[r][c+1]==EMPTY && board[r][c+2]==EMPTY
                         && !squareAttacked(r, c+1, !whiteToMove)
                         && !squareAttacked(r, c+2, !whiteToMove))
                             m.push_back({r,c,r,c+2,0});


                    if(!rookLMoved[s] && board[r][c-1]==EMPTY && board[r][c-2]==EMPTY && board[r][c-3]==EMPTY
                        && !squareAttacked(r, c-1, !whiteToMove)
                        && !squareAttacked(r, c-2, !whiteToMove))
                            m.push_back({r,c,r,c-2,0});
                }
            }
        }
    }


    return m;
}

    vector<Move> generateLegalMoves() {
    vector<Move> all = generateMoves(), legal;
    for(auto& mv : all) {
        Undo u = makeMove(mv);
        if(!inCheck(!whiteToMove)) legal.push_back(mv);
        undoMove(mv, u);
    }
    return legal;
}


vector<Move> generateCaptures() {
    vector<Move> m;
    for(int r = 0; r < 8; r++) {
        for(int c = 0; c < 8; c++) {
            Piece p = board[r][c];
            if(p == EMPTY || isWhite(p) != whiteToMove) continue;

            if (p == WP || p == BP) {
                int dir = whiteToMove ? -1 : 1;
                int promoRow = whiteToMove ? 0 : 7;
                int nextR = r + dir;

                for (int dc : {-1, 1}) {
                    int nc = c + dc;
                    if (inBounds(nextR, nc)) {

                        if (board[nextR][nc] != EMPTY && isWhite(board[nextR][nc]) != whiteToMove) {
                            if (nextR == promoRow) {
                                m.push_back(Move(r, c, nextR, nc, 1, whiteToMove ? WQ : BQ));
                            } else {
                                m.push_back(Move(r, c, nextR, nc, 0));
                            }
                        }

                        if (r == (whiteToMove ? 3 : 4) && nc == enPassantCol) {
                            m.push_back(Move(r, c, nextR, nc, 2));
                        }
                    }
                }
            }
            else if(p == WN || p == BN) {
                int dr[]={-2,-2,-1,-1,1,1,2,2}, dc[]={-1,1,-2,2,-2,2,-1,1};
                for(int i = 0; i < 8; i++) {
                    int nr = r + dr[i], nc = c + dc[i];
                    if(inBounds(nr, nc) && board[nr][nc] != EMPTY && isWhite(board[nr][nc]) != whiteToMove)
                        m.push_back({r, c, nr, nc, 0});
                }
            }
            else {
                vector<pair<int,int>> dirs;
                if(p == WB || p == BB) dirs = {{-1,-1},{-1,1},{1,-1},{1,1}};
                else if(p == WR || p == BR) dirs = {{-1,0},{1,0},{0,-1},{0,1}};
                else if(p == WQ || p == BQ || p == WK || p == BK)
                    dirs = {{-1,-1},{-1,1},{1,-1},{1,1},{-1,0},{1,0},{0,-1},{0,1}};

                for(auto& dr : dirs) {
                    int nr = r + dr.first, nc = c + dr.second;
                    while(inBounds(nr, nc)) {
                        if(board[nr][nc] != EMPTY) {
                            if(isWhite(board[nr][nc]) != whiteToMove)
                                m.push_back({r, c, nr, nc, 0});
                            break;
                        }
                        if (p == WK || p == BK) break;

                        nr += dr.first; nc += dr.second;
                    }
                }
            }
        }
    }
    return m;
}


vector<Move> generateLegalCaptures() {
    vector<Move> all = generateCaptures(), legal;
    for(auto& mv : all) {
        Undo u = makeMove(mv);
        if(!inCheck(!whiteToMove)) legal.push_back(mv);
        undoMove(mv, u);
    }
    return legal;
}