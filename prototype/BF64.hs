{-# LANGUAGE RecursiveDo #-}
module BF64 (main) where

import Language.Brainfuck.Syntax
import Language.Brainfuck.Parser

import Control.Monad.RWS

import System.Environment (getArgs, getProgName)
import System.Exit
import System.IO

import Data.Word
import qualified Data.ByteString as BS

type Addr = (Word8, Word8)
type Compile = RWS Addr [Word8] Addr

header :: [Word8]
header = [ 0x01, 0x08 -- Origin
         , 0x0b, 0x08
         , 0xd9, 0x02, 0x9e -- 729 SYS
         , 0x32, 0x30, 0x36, 0x31 -- 2061
         , 0x00, 0x00, 0x00
         ]

prelude :: Compile ()
prelude = do
    (memLo, memHi) <- askMem
    emit [ 0xa9, 0x17         -- LDA #$17
         , 0x8d, 0x18, 0xd0   -- STA $D018
         , 0xa9, 0x00         -- LDA #$00
         , 0xa2, 0xff         -- LDX #$ff
         , 0x9d, memLo, memHi -- STA MEM,X -- loop
         , 0xca               -- DEX
         , 0xd0, 0xfa         -- BNE loop
         ]
compile :: Program -> Compile ()
compile = mapM_ compile1

compile1 :: Stmt -> Compile ()
compile1 stmt = do
    (memLo, memHi) <- askMem
    case stmt of
        IncData ->
            emit [ 0xfe, memLo, memHi ] -- INC MEM,X
        DecData ->
            emit [ 0xde, memLo, memHi ] -- DEC MEM,X
        IncPtr ->
            emit [ 0xe8 ] -- INX
        DecPtr ->
            emit [ 0xca ] -- DEX
        Output ->
            emit [ 0xbd, memLo, memHi -- LDA MEM,X
                 , 0x20, 0xd2, 0xff   -- JSR $FFD2
                 ]
        While prog -> do
            (startLo, startHi) <- getPC
            rec
              emit [ 0xbd, memLo, memHi -- LDA MEM,X
                   , 0xd0, 0x03         -- BNE +3
                   , 0x4c, endLo, endHi -- JMP end
                   ]
              compile prog
              emit [ 0x4c, startLo, startHi ] -- JMP start
              (endLo, endHi) <- getPC
            return ()

emit :: [Word8] -> Compile ()
emit bs = do
    tell bs
    modify $ bumpBy (length bs)
  where
    bumpBy :: Int -> (Word8, Word8) -> (Word8, Word8)
    bumpBy n = from . (+ fromIntegral n) . to
      where
        to :: (Word8, Word8) -> Word16
        to (lo, hi) = fromIntegral hi * 256 + fromIntegral lo

        from :: Word16 -> (Word8, Word8)
        from x = swap . both fromIntegral $ x `divMod` 256

        both f (x, y) = (f x, f y)
        swap (x, y) = (y, x)

askMem :: Compile Addr
askMem = ask

getPC :: Compile Addr
getPC = get

main :: IO ()
main = do
    args <- getArgs
    filename <- case args of
        [filename] -> return filename
        _ -> do
            self <- getProgName
            hPutStrLn stderr $ unwords ["Usage:", self, "filename.bf"]
            exitWith $ ExitFailure 1

    parseRes <- parseBrainFuck filename
    bf <- case parseRes of
        Left err -> do
            hPutStrLn stderr $ show err
            exitWith $ ExitFailure 2
        Right bf -> return bf

    let mem = (0x00, 0x20)
    let (_, asm) = execRWS (prelude >> compile bf >> emit [ 0x60 ]) mem (0x0d, 0x08)
        prg = BS.pack $ header ++ asm

    BS.writeFile "test.prg" prg
