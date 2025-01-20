#include "bits/stdc++.h"


class Chess {
    
    private:
    
    enum ColorType {
        
        WHITE = 0,
        BLACK = 1,
        NONE = -1
    };
    static std::string enumToString(ColorType color) {
        
        if (color == WHITE) return "WHITE";
        else if (color == BLACK) return "BLACK";
        else return "NONE";
    }
    static ColorType oppositeColor(ColorType turn) {
        
        if (turn == WHITE) return BLACK;
        else return WHITE;
    }
    enum PieceType {
        
        KING = 0,
        QUEEN = 1,
        ROOK = 2,
        KNIGHT = 3,
        BISHOP = 4,
        PAWN = 5,
        EMPTY = -1
    };
    static std::string enumToString(PieceType type) {
        
        if (type == KING) return "KING";
        else if (type == QUEEN) return "QUEEN";
        else if (type == ROOK) return "ROOK";
        else if (type == KNIGHT) return "KNIGHT";
        else if (type == BISHOP) return "BISHOP";
        else if (type == PAWN) return "PAWN";
        else return "EMPTY";
    }
    class ChessPiece {
        
        private:
        
        ColorType color;
        PieceType type;
        
        public:
        
        ChessPiece() : color(NONE), type(EMPTY) {}
        ChessPiece(ColorType inputColor, PieceType inputType) : color(inputColor), type(inputType) {}
        void draw() const {
            
            switch (type) {
                
                case KING:
                
                if (color == WHITE) std::cout << "♔ "; 
                else if (color == BLACK) std::cout << "♚ ";
                break;
                
                case QUEEN:
                
                if (color == WHITE) std::cout << "♕ "; 
                else if (color == BLACK) std::cout << "♛ ";
                break;
                
                case ROOK:
                
                if (color == WHITE) std::cout << "♖ "; 
                else if (color == BLACK) std::cout << "♜ ";
                break;
                
                case KNIGHT:
                
                if (color == WHITE) std::cout << "♘ "; 
                else if (color == BLACK) std::cout << "♞ ";
                break;
                
                case BISHOP:
                
                if (color == WHITE) std::cout << "♗ "; 
                else if (color == BLACK) std::cout << "♝ ";
                break;
                
                case PAWN:
                
                if (color == WHITE) std::cout << "♙ "; 
                else if (color == BLACK) std::cout << "♟ ";
                break;
                
                default:
                
                std::cout << "  ";
                break;
            }
        }
        void execute() {
            
            this->color = NONE;
            this->type = EMPTY;
        }
        void promote(PieceType toPromote) {
            
            this->type = toPromote;
        }
        bool isBlank() const {
            
            return this->color == NONE;
        }
        ColorType getColor() const {
            
            return this->color;
        }
        PieceType getType() const {
            
            return this->type;
        }
    };
    struct Square {
        
        int rank = -1, file = -1;
        
        Square() = default;
        Square(int inputRank, int inputFile) : rank(inputRank), file(inputFile) {}
        
        bool operator<(const Square& rhs) const {
            
            if (this->rank == rhs.rank) return this->file < rhs.file;
            return this->rank < rhs.rank;
        }
    };
    
    // Chess class member variables
    static const int RANK_MAX = 8, FILE_MAX = 8;
    bool gameOver = false, noticeCheck = true;
    ColorType turn = WHITE;
    PieceType lastMoveType = EMPTY;
    Square lastMoveFrom, lastMoveTo;
    std::array<bool, 2> isCheck = {false, false};
    std::array<std::array<bool, 2>, 2> doesCastlingInterrupted = {{{false, false}, {false, false}}};
    // doesCastlingInterrupted[ColorType][PieceType]; ColorType : WHITE / BLACK, PieceType : KING(side), QUEEN(side)
    std::array<bool, 2> doesKingMoved = {false, false};
    std::array<std::array<bool, 2>, 2> doesRookMoved = {{{false, false}, {false, false}}};
    // doesRookMoved[ColorType][PieceType]; ColorType : WHITE / BLACK, PieceType : KING(side), QUEEN(side)
    std::array<std::array<ChessPiece, FILE_MAX>, RANK_MAX> chessBoard;
    std::array<std::array<std::vector<Square>, FILE_MAX>, RANK_MAX> candidateMoves, availableMoves;
    /*
    candidateMoves는
    1) 현재가 체크 상태인데 그 move가 체크 상태를 해소하지 못하는 경우
    2) 해당 기물이 핀에 걸려있어 움직일 수 없는 경우
    3) 움직이고자 하는 기물이 킹인데 킹이 제발로 죽으러 가는 경우
    등을 고려하지 않은, 행마법 상 가능한 모든 수들의 집합 
    
    availableMoves는 이들을 전부 고려한 "실제로" 가능한 수들의 집합
    
    <둘을 나눈 이유>
    현재 상태에서 availableMoves를 계산하기 위해선
    1) 우선 내 입장에서의 candidateMoves를 구한 뒤
    2) candidateMoves의 원소에 해당하는 각 move를 했다고 가정하고
    3) 이 때 체스판 상황에서 상대 입장에서의 candidateMoves를 구했을 때
    4) 자신의 킹이 위치하는 칸이 상대의 candidateMoves에 들어간다면 이는 불가능한 수.
    즉 candidateMoves에서 이러한 수들을 제외한 나머지가 availableMoves가 된다.
    */
    
    static Square convert(const std::string& s) {
        
        return Square((int)(s[1] - '1'), (int)(s[0] - 'a'));
    }
    static std::string convert(const Square& square) {
        
        std::string s;
        
        s += (char)('a' + square.file);
        s += (char)('1' + square.rank);
        
        return s;
    }
    void init() {
        
        // initialize member variables
        this->gameOver = false;
        this->turn = WHITE;
        this->lastMoveType = EMPTY;
        this->lastMoveTo = Square();
        this->lastMoveFrom = Square();
        this->isCheck = {false, false};
        this->doesCastlingInterrupted = {{{false, false}, {false, false}}};
        this->doesKingMoved = {false, false};
        this->doesRookMoved = {{{false, false}, {false, false}}};
        
        for (int rank = 0; rank < 8; ++rank) {
            
            for (int file = 0; file < 8; ++file) {
                
                candidateMoves[rank][file].clear();
                availableMoves[rank][file].clear(); 
            }
        }
        
        // initialize chessBoard
        
        // rank 1
        chessBoard[0][0] = ChessPiece(WHITE, ROOK);
        chessBoard[0][1] = ChessPiece(WHITE, KNIGHT);
        chessBoard[0][2] = ChessPiece(WHITE, BISHOP);
        chessBoard[0][3] = ChessPiece(WHITE, QUEEN);
        chessBoard[0][4] = ChessPiece(WHITE, KING);
        chessBoard[0][5] = ChessPiece(WHITE, BISHOP);
        chessBoard[0][6] = ChessPiece(WHITE, KNIGHT);
        chessBoard[0][7] = ChessPiece(WHITE, ROOK);
        
        // rank 2
        for (int file = 0; file < 8; ++file) {
            
            chessBoard[1][file] = ChessPiece(WHITE, PAWN);
        }
        
        // rank 3~6
        for (int rank = 2; rank < 6; ++rank) {
            
            for (int file = 0; file < 8; ++file) {
                
                chessBoard[rank][file] = ChessPiece();
            }
        }
        
        // rank 7
        for (int file = 0; file < 8; ++file) {
            
            chessBoard[6][file] = ChessPiece(BLACK, PAWN);
        }
        
        // rank 8
        chessBoard[7][0] = ChessPiece(BLACK, ROOK);
        chessBoard[7][1] = ChessPiece(BLACK, KNIGHT);
        chessBoard[7][2] = ChessPiece(BLACK, BISHOP);
        chessBoard[7][3] = ChessPiece(BLACK, QUEEN);
        chessBoard[7][4] = ChessPiece(BLACK, KING);
        chessBoard[7][5] = ChessPiece(BLACK, BISHOP);
        chessBoard[7][6] = ChessPiece(BLACK, KNIGHT);
        chessBoard[7][7] = ChessPiece(BLACK, ROOK);
    }
    PieceType getPromotionInput() {
        
        std::string userInput;
        
        std::cout << "\nPromotion!\n";
        
        while (true) {
            
            std::cout << "\nChoose piece type to promote (1: Queen / 2: Rook / 3: Knight / 4 : Bishop) : ";
            std::getline(std::cin, userInput);
            
            if (userInput == "1") {
                
                return QUEEN;
            }
            else if (userInput == "2") {
                
                return ROOK;
            }
            else if (userInput == "3") {
                
                return KNIGHT;
            }
            else if (userInput == "4") {
                
                return BISHOP;
            }
            else {
                
                std::cout << "\nInvalid input. Please enter again.\n";
            }
        }
    }
    void move(int rank, int file, int destRank, int destFile, bool realMove = true) {
        
        ChessPiece& cursor = this->chessBoard[rank][file];
        ChessPiece& dest = this->chessBoard[destRank][destFile];
        
        const ColorType cursorColor = cursor.getColor();
        const PieceType cursorType = cursor.getType();
        
        if (!dest.isBlank()) {
            
            dest.execute();
        }
        else if (cursorType == PAWN) {
            
            // En passant
            if (file != destFile) {
            
                this->chessBoard[rank][destFile].execute();
            }
        }
        else if (cursorType == KING) {
            
            // King side castling
            if (destFile - file == 2) {
                
                // Move rook
                std::swap(this->chessBoard[rank][7], this->chessBoard[rank][5]);
            }
            
            // Queen side castling
            if (destFile - file == -2) {
                
                // Move rook
                std::swap(this->chessBoard[rank][0], this->chessBoard[rank][3]);
            }
        }
        
        std::swap(cursor, dest);
        
        this->lastMoveType = cursorType;
        this->lastMoveFrom = {rank, file};
        this->lastMoveTo = {destRank, destFile};
        
        if (realMove) {
            
            if (cursorType == PAWN) {
                
                // promotion
                if ((cursorColor == WHITE && destRank == 7) || (cursorColor == BLACK && destRank == 0)) {
                    
                    this->chessBoard[destRank][destFile] = ChessPiece(cursorColor, this->getPromotionInput());
                }
            }
            
            if (cursorType == KING) {
                
                doesKingMoved[cursorColor] = true;
            }
            else if (cursorType == ROOK) {
                
                if (rank == 0 && file == 0) doesRookMoved[WHITE][QUEEN] = true;
                else if (rank == 0 && file == 7) doesRookMoved[WHITE][KING] = true;
                else if (rank == 7 && file == 0) doesRookMoved[BLACK][QUEEN] = true;
                else if (rank == 7 && file == 7) doesRookMoved[BLACK][KING] = true;
            }
        }
    }
    bool isCastlingAvailable(ColorType color, PieceType type) {
        
        int kingRank;
        
        if (color == WHITE) kingRank = 0;
        else if (color == BLACK) kingRank = 7;
        
        if (type == KING) {
            
            if (!this->chessBoard[kingRank][5].isBlank() || !this->chessBoard[kingRank][6].isBlank()) {
                
                return false;
            }
        }
        else if (type == QUEEN) {
            
            if (!this->chessBoard[kingRank][3].isBlank() || !this->chessBoard[kingRank][2].isBlank() || !this->chessBoard[kingRank][1].isBlank()) {
                
                return false;
            }
        }
        
        return !doesCastlingInterrupted[color][type] && !doesRookMoved[color][type] && !doesKingMoved[color];
    }
    void draw() const {
        
        std::cout << "\nWHITE PERSPECTIVE\tBLACK PERSPECTIVE\n\n";
        
        for (int rank = 0; rank < 8; ++rank) {
            
            // white perspective
            std::cout << 8 - rank << ' ';
            for (int file = 0; file < 8; ++file) {
                
                this->chessBoard[7 - rank][file].draw();
            }
            std::cout << '\t';
            
            // black perspective
            std::cout << rank + 1 << ' ';
            for (int file = 0; file < 8; ++file) {
                
                this->chessBoard[rank][7 - file].draw();
            }
            std::cout << '\n';
        }
        
        std::cout << "  a b c d e f g h\t  h g f e d c b a\n";
        
        if (this->lastMoveType != EMPTY) {
            
            std::cout << "\nLast Move : " << enumToString(oppositeColor(this->turn)) << ' ' <<  enumToString(lastMoveType);
            std::cout << ", " << convert(this->lastMoveFrom) << " -> " << convert(this->lastMoveTo) << "\n";
        }
        
        if (this->noticeCheck && this->isCheck[this->turn]) {
            
            std::cout << "\nCHECK!\n";
        }
        
        std::cout << "\n\n";
        
        if (this->gameOver) {
            
            std::cout << "\nGAME OVER\n\n";
            
            if (this->isCheck[this->turn]) {
                
                std::cout << "CHECKMATE! " << enumToString(oppositeColor(this->turn)) << " WIN!\n\n\n";
            }
            else {
                
                std::cout << "STALEMATE!\n\n\n";
            }
        }
        /*
        std::cout << "gameOver: " << gameOver << '\n';
        std::cout << "turn: " << enumToString(turn) << '\n';
        std::cout << "lastMoveType: " << enumToString(lastMoveType) << '\n';
        std::cout << "lastMoveFrom: " << convert(lastMoveFrom) << '\n';
        std::cout << "lastMoveTo: " << convert(lastMoveTo) << '\n';
        std::cout << "isCheck: {" << isCheck[0] << ", " << isCheck[1] << "}\n";
        std::cout << "doesCastlingInterrupted: {{" << doesCastlingInterrupted[0][0] << ", " << doesCastlingInterrupted[0][1] << "}, {";
        std::cout << doesCastlingInterrupted[1][0] << ", " << doesCastlingInterrupted[1][1] << "}}\n";
        std::cout << "doesKingMoved: {" << doesKingMoved[0] << ", " << doesKingMoved[1] << "}\n";
        std::cout << "doesRookMoved: {{" << doesRookMoved[0][0] << ", " << doesRookMoved[0][1] << "}, {";
        std::cout << doesRookMoved[1][0] << ", " << doesRookMoved[1][1] << "}}\n\n";
        
        for (int rank = 0; rank < RANK_MAX; ++rank) {
            
            for (int file = 0; file < FILE_MAX; ++file) {
                
                const ChessPiece& cursor = chessBoard[rank][file];
                
                if (cursor.isBlank()) continue;
                
                std::cout << convert(Square(rank, file)) << "\tCandidate : ";
                
                for (const auto& move : candidateMoves[rank][file]) std::cout << convert(move) << ' ';
                
                std::cout << "\n\tAvailable : ";
                
                for (const auto& move : availableMoves[rank][file]) std::cout << convert(move) << ' ';
                
                std::cout << "\n\n";
            }
        }
        std::cout << '\n';
        */
    }
    void getCandidateMoves() {
        
        for (int rank = 0; rank < 8; ++rank) {
            
            for (int file = 0; file < 8; ++file) {
                
                this->candidateMoves[rank][file].clear();
                
                const ChessPiece& cursor = this->chessBoard[rank][file];
                
                const ColorType cursorColor = cursor.getColor();
                const PieceType cursorType = cursor.getType();
                
                // pawn exception
                if (cursorType == PAWN) {
                    
                    switch (cursorColor) {
                        
                        case WHITE:
                        
                        if (rank < 7 && chessBoard[rank + 1][file].isBlank()) {
                            
                            this->candidateMoves[rank][file].push_back({rank + 1, file});
                            
                            // first move
                            if (rank == 1 && chessBoard[rank + 2][file].isBlank()) {
                                
                                this->candidateMoves[rank][file].push_back({rank + 2, file});
                            }
                        }
                        
                        // attack
                        if (rank < 7) {
                            
                            if (file != 0 && chessBoard[rank + 1][file - 1].getColor() == oppositeColor(cursorColor)) {
                                
                                this->candidateMoves[rank][file].push_back({rank + 1, file - 1});
                                
                                if (this->chessBoard[rank + 1][file - 1].getType() == KING) {
                                    
                                    this->isCheck[BLACK] = true;
                                }
                            }
                            
                            if (file != 7 && chessBoard[rank + 1][file + 1].getColor() == oppositeColor(cursorColor)) {
                                
                                this->candidateMoves[rank][file].push_back({rank + 1, file + 1});
                                
                                if (this->chessBoard[rank + 1][file + 1].getType() == KING) {
                                    
                                    this->isCheck[BLACK] = true;
                                }
                            }
                        }
                        
                        // En passant
                        if (rank == 4 && lastMoveType == PAWN) {
                            
                            if (lastMoveFrom.rank == 6 && lastMoveTo.rank == 4) {
                                
                                if (lastMoveTo.file - file == 1 || lastMoveTo.file - file == -1) {
                                    
                                    this->candidateMoves[rank][file].push_back({rank + 1, lastMoveTo.file});
                                }
                            }
                        }
                        
                        // castling interruption check
                        if (rank == 6) {
                            
                            if (file >= 1 && file <= 5) {
                                
                                doesCastlingInterrupted[BLACK][QUEEN] = true;
                            }
                            
                            if (file >= 3 && file <= 7) {
                                
                                doesCastlingInterrupted[BLACK][KING] = true;
                            }
                        }
                        
                        break;
                        
                        case BLACK:
                        
                        if (rank > 0 && chessBoard[rank - 1][file].isBlank()) {
                            
                            this->candidateMoves[rank][file].push_back({rank - 1, file});
                            
                            // first move
                            if (rank == 6 && chessBoard[rank - 2][file].isBlank()) {
                                
                                this->candidateMoves[rank][file].push_back({rank - 2, file});
                            }
                        }
                        
                        // attack
                        if (rank > 0) {
                            
                            if (file != 0 && chessBoard[rank - 1][file - 1].getColor() == oppositeColor(cursorColor)) {
                                
                                this->candidateMoves[rank][file].push_back({rank - 1, file - 1});
                                
                                if (this->chessBoard[rank - 1][file - 1].getType() == KING) {
                                    
                                    this->isCheck[WHITE] = true;
                                }
                            }
                            
                            if (file != 7 && chessBoard[rank - 1][file + 1].getColor() == oppositeColor(cursorColor)) {
                                
                                this->candidateMoves[rank][file].push_back({rank - 1, file + 1});
                                
                                if (this->chessBoard[rank - 1][file + 1].getType() == KING) {
                                    
                                    this->isCheck[WHITE] = true;
                                }
                            }
                        }
                        
                        // En passant
                        if (rank == 3 && lastMoveType == PAWN) {
                            
                            if (lastMoveFrom.rank == 1 && lastMoveTo.rank == 3) {
                                
                                if (lastMoveTo.file - file == 1 || lastMoveTo.file - file == -1) {
                                    
                                    this->candidateMoves[rank][file].push_back({rank - 1, lastMoveTo.file});
                                }
                            }
                        }
                        
                        // castling interruption check
                        if (rank == 1) {
                            
                            if (file >= 1 && file <= 5) {
                                
                                doesCastlingInterrupted[WHITE][QUEEN] = true;
                            }
                            
                            if (file >= 3 && file <= 7) {
                                
                                doesCastlingInterrupted[WHITE][KING] = true;
                            }
                        }
                        
                        break;
                    }
                }
                else {
                    
                    std::vector<Square> possibleMoves;
                    bool goFurther;
                    
                    switch (cursorType) {
                        
                        case KING:
                        
                        possibleMoves = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}, {1, 1}, {-1, 1}, {-1, -1}, {1, -1}};
                        goFurther = false;
                        break;
                        
                        case QUEEN:
                        
                        possibleMoves = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}, {1, 1}, {-1, 1}, {-1, -1}, {1, -1}};
                        goFurther = true;
                        break;
                        
                        case ROOK:
                        
                        possibleMoves = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
                        goFurther = true;
                        break;
                        
                        case KNIGHT:
                        
                        possibleMoves = {{2, 1}, {1, 2}, {-1, 2}, {-2, 1}, {-2, -1}, {-1, -2}, {1, -2}, {2, -1}};
                        goFurther = false;
                        break;
                        
                        case BISHOP:
                        
                        possibleMoves = {{1, 1}, {-1, 1}, {-1, -1}, {1, -1}};
                        goFurther = true;
                        break;
                        
                        default:
                        
                        break;
                    }
                    
                    for (const Square& move : possibleMoves) {
                        
                        bool _goFurther = goFurther;
                        
                        int destRank = rank, destFile = file;
                        
                        do {
                            
                            destRank = destRank + move.rank;
                            destFile = destFile + move.file;
                            
                            if (destRank < 0 || destRank >= RANK_MAX || destFile < 0 || destFile >= FILE_MAX) {
                                
                                _goFurther = false;
                            }
                            else if (this->chessBoard[destRank][destFile].getColor() == cursorColor) {
                                
                                _goFurther = false;
                            }
                            else {
                                
                                this->candidateMoves[rank][file].push_back({destRank, destFile});
                                
                                if (this->chessBoard[destRank][destFile].getColor() == oppositeColor(cursorColor)) {
                                    
                                    _goFurther = false;
                                    
                                    if (this->chessBoard[destRank][destFile].getType() == KING) {
                                        
                                        this->isCheck[oppositeColor(cursorColor)] = true;
                                    }
                                }
                                
                                // castling interruption check
                                switch (cursorColor) {
                                    
                                    case WHITE:
                                    
                                    if (destRank == 7) {
                                        
                                        if (destFile == 4) {
                                            
                                            doesCastlingInterrupted[BLACK][KING] = true;
                                            doesCastlingInterrupted[BLACK][QUEEN] = true;
                                        }
                                        else if (destFile == 2 || destFile == 3) {
                                            
                                            doesCastlingInterrupted[BLACK][QUEEN] = true;
                                        }
                                        else if (destFile == 5 || destFile == 6) {
                                            
                                            doesCastlingInterrupted[BLACK][KING] = true;
                                        }
                                    }
                                    break;
                                    
                                    case BLACK:
                                    
                                    if (destRank == 0) {
                                        
                                        if (destFile == 4) {
                                            
                                            doesCastlingInterrupted[WHITE][KING] = true;
                                            doesCastlingInterrupted[WHITE][QUEEN] = true;
                                        }
                                        else if (destFile == 2 || destFile == 3) {
                                            
                                            doesCastlingInterrupted[WHITE][QUEEN] = true;
                                        }
                                        else if (destFile == 5 || destFile == 6) {
                                            
                                            doesCastlingInterrupted[WHITE][KING] = true;
                                        }
                                    }
                                    break;
                                    
                                    default:
                                    
                                    break;
                                }
                            }
                            
                        } while (_goFurther);
                    }
                }
            }
        }
        
        // Castling
        if (isCastlingAvailable(WHITE, KING)) {
            
            this->candidateMoves[0][4].push_back({0, 6});
        }

        if (isCastlingAvailable(WHITE, QUEEN)) {
            
            this->candidateMoves[0][4].push_back({0, 2});
        }
            
        if (isCastlingAvailable(BLACK, KING)) {
            
            this->candidateMoves[7][4].push_back({7, 6});
        }

        if (isCastlingAvailable(BLACK, QUEEN)) {
            
            this->candidateMoves[7][4].push_back({7, 2});
        }
    }
    void getAvailableMoves() {
        
        bool isAvailableMoveExist = false;
        
        for (int rank = 0; rank < 8; ++rank) {
            
            for (int file = 0; file < 8; ++file) {
                
                this->availableMoves[rank][file].clear();
                
                const ChessPiece& cursor = this->chessBoard[rank][file];
                
                const ColorType& cursorColor = cursor.getColor();
                const PieceType& cursorType = cursor.getType();
                
                if (cursorColor != this->turn) continue;
                
                for (auto& [destRank, destFile] : this->candidateMoves[rank][file]) {
                    
                    Chess nextCircumstance(*this);
                    
                    nextCircumstance.move(rank, file, destRank, destFile, false);
                    nextCircumstance.getCandidateMoves();
                    
                    if (!nextCircumstance.isCheck[this->turn]) {
                        
                        this->availableMoves[rank][file].push_back({destRank, destFile});
                        isAvailableMoveExist = true;
                    }
                }
                
                std::sort(availableMoves[rank][file].begin(), availableMoves[rank][file].end());
            }
        }
        
        if (!isAvailableMoveExist) this->gameOver = true;
    }
    void getUserInput() {
        
        std::string userInput;
        
        while (true) {
            
            std::cout << '\n' << enumToString(this->turn) << " turn. Select piece to move : ";
            std::getline(std::cin, userInput);
            
            if (userInput == "resign") {
                
                while (true) {
                    
                    std::cout << "\nAre you sure to resign? (Y/N) : ";
                    std::getline(std::cin, userInput);
                    
                    if (userInput == "Y") {
                        
                        std::cout << "\n\n\n" << enumToString(this->turn) << " resigned.\n\n\n";
                        this->gameOver = true;
                        return;
                    }
                    else if (userInput == "N") {
                        
                        break;
                    }
                    else {
                        
                        std::cout << "\nWrong input. Please enter again.\n";
                    }
                }
                
                continue;
            }
            
            if (userInput.length() == 2) {
                
                auto [rank, file] = convert(userInput);
                
                if (rank >= 0 && rank < RANK_MAX && file >= 0 && file < FILE_MAX) {
                    
                    if (this->chessBoard[rank][file].getColor() == this->turn && !this->availableMoves[rank][file].empty()) {
                        
                        std::cout << "\nPossible destinations : ";
                        
                        for (auto [destRank, destFile] : this->availableMoves[rank][file]) {
                            
                            std::cout << convert(Square(destRank, destFile)) << ' ';
                        }
                        
                        std::cout << '\n';
                        
                        bool isCanceled = false;
                            
                        while (true) {
                            
                            std::cout << "\nSelect square to move, or enter 'cancel' to select another piece : ";
                            std::getline(std::cin, userInput);
                            
                            if (userInput == "cancel") {
                                
                                isCanceled = true;
                                break;
                            }
                            
                            if (userInput.length() == 2) {
                                
                                auto [_destRank, _destFile] = convert(userInput);
                                
                                if (_destRank >= 0 && _destRank < RANK_MAX && _destFile >= 0 && _destFile < FILE_MAX) {
                                    
                                    for (auto [destRank, destFile] : this->availableMoves[rank][file]) {
                                        
                                        if (_destRank == destRank && _destFile == destFile) {
                                            
                                            move(rank, file, destRank, destFile);
                                            this->turn = oppositeColor(this->turn);
                                            std::cout << "\n\n";
                                            
                                            return;
                                        }
                                    }
                                }
                            }
                            
                            std::cout << "\nInvalid input. Please enter again.\n";
                        }
                        
                        if (isCanceled) {
                            
                            std::cout << "\nPiece select canceled.\n";
                            continue;
                        }
                    }
                }
            }
                
            std::cout << "\nInvalid input. Please enter again.\n";
        }
    }
    void play() {
        
        while (true) {
            
            this->getCandidateMoves();
            this->getAvailableMoves();
            this->draw();
            
            if (this->gameOver) break;
            
            this->getUserInput();
            
            if (this->gameOver) break;
        }
    }
    bool determineRestart() {
        
        std::string userInput;
        
        while (true) {
            
            std::cout << "\nRestart? (Y/N) : ";
            std::getline(std::cin, userInput);
            
            if (userInput == "Y") {
                
                std::cout << "\n\n\n";
                return true;
            }
            else if (userInput == "N") {
                
                std::cout << "\n\n\nTHANK YOU FOR PLAYING!\n\n\n";
                return false;
            }
            else {
                
                std::cout << "\nWrong input. Please enter again.\n";
            }
        }
    }
    
    Chess(const Chess& toDuplicate) {
        
        this->lastMoveType = toDuplicate.lastMoveType;
        this->lastMoveFrom = toDuplicate.lastMoveFrom;
        this->lastMoveTo = toDuplicate.lastMoveTo;
        this->doesKingMoved = toDuplicate.doesKingMoved;
        this->doesRookMoved = toDuplicate.doesRookMoved;
        this->turn = oppositeColor(toDuplicate.turn);
        
        for (int rank = 0; rank < 8; ++rank) {
            
            for (int file = 0; file < 8; ++file) {
                
                this->chessBoard[rank][file] = toDuplicate.chessBoard[rank][file];
            }
        }
    }
    
    public:
    
    Chess() = default;
    
    void start() {
        
        bool restartFlag = true;
        
        do {
            
            this->init();
            
            this->play();
            
            restartFlag = this->determineRestart();
            
        } while (restartFlag);
    }
};




int main(int argc, char** argv) {
    
    Chess myChess;
    myChess.start();
    
    return 0;
}
