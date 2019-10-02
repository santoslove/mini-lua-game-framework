engine.width = 400
engine.height = 300
engine.scale = 2

images = {}

function takeCard(hand)
    table.insert(hand, table.remove(deck, math.random(#deck)))
end

function getTotal(hand)
    local total = 0
    local hasAce = false

    for cardIndex, card in ipairs(hand) do
        if card.rank > 10 then
            total = total + 10
        else
            total = total + card.rank
        end

        if card.rank == 1 then
            hasAce = true
        end
    end

    if hasAce and total <= 11 then
        total = total + 10
    end

    return total
end

buttonY = 230
buttonHeight = 25

buttonHitX = 10
buttonHitWidth = 53

buttonStandX = 70
buttonStandWidth = 53

buttonPlayAgainX = 10
buttonPlayAgainWidth = 113

function isMouseInButton(buttonX, buttonWidth)
    return engine.mouseX() >= buttonX
    and engine.mouseX() < buttonX + buttonWidth
    and engine.mouseY() >= buttonY
    and engine.mouseY() < buttonY + buttonHeight
end

function reset()
    deck = {}
    for suitIndex, suit in ipairs({'club', 'diamond', 'heart', 'spade'}) do
        for rank = 1, 13 do
            table.insert(deck, {suit = suit, rank = rank})
        end
    end

    playerHand = {}
    takeCard(playerHand)
    takeCard(playerHand)

    dealerHand = {}
    takeCard(dealerHand)
    takeCard(dealerHand)

    roundOver = false
end

reset()
    
function engine.frame()
    local output = {}

    engine.text(table.concat(output, '\n'), 'Vera.ttf', 12, 15, 15)

    if roundOver then
        table.insert(output, '')

        local function hasHandWon(thisHand, otherHand)
            return getTotal(thisHand) <= 21
            and (
                getTotal(otherHand) > 21
                or getTotal(thisHand) > getTotal(otherHand)
            )
        end

        if hasHandWon(playerHand, dealerHand) then
            table.insert(output, 'Player wins')
        elseif hasHandWon(dealerHand, playerHand) then
            table.insert(output, 'Dealer wins')
        else
            table.insert(output, 'Draw')
        end
    end

    local function drawCard(card, x, y)
        engine.image('blackjack_images/card.png', x, y)

        local color
        if card.suit == 'heart' or card.suit == 'diamond' then
            color = {228, 15, 100}
        else
            color = {50, 50, 50}
        end

        local cardWidth = 53
        local cardHeight = 73

        local function drawCorner(image, offsetX, offsetY)
            engine.image(
                'blackjack_images/'..image,
                x + offsetX,
                y + offsetY,
                0,
                1,
                1,
                0,
                0,
                color[1],
                color[2],
                color[3]
            )
            engine.image(
                'blackjack_images/'..image,
                x + cardWidth - offsetX,
                y + cardHeight - offsetY,
                0,
                -1,
                -1,
                0,
                0,
                color[1],
                color[2],
                color[3]
            )
        end

        drawCorner(card.rank..'.png', 3, 4)
        drawCorner('mini_'..card.suit..'.png', 3, 14)

        if card.rank > 10 then
            local faceImage
            if card.rank == 11 then
                faceImage = 'face_jack.png'
            elseif card.rank == 12 then
                faceImage = 'face_queen.png'
            elseif card.rank == 13 then
                faceImage = 'face_king.png'
            end
            engine.image('blackjack_images/'..faceImage, x + 12, y + 11)
        else
            local function drawPip(offsetX, offsetY, mirrorX, mirrorY)
                local pipImage = 'pip_'..card.suit..'.png'
                local pipWidth = 11
                engine.image(
                    'blackjack_images/'..pipImage,
                    x + offsetX,
                    y + offsetY,
                    0,
                    1,
                    1,
                    0,
                    0,
                    color[1],
                    color[2],
                    color[3]
                )
                if mirrorX then
                    engine.image(
                        'blackjack_images/'..pipImage,
                        x + cardWidth - offsetX - pipWidth,
                        y + offsetY,
                        0,
                        1,
                        1,
                        0,
                        0,
                        color[1],
                        color[2],
                        color[3]
                    )
                end
                if mirrorY then
                    engine.image(
                        'blackjack_images/'..pipImage,
                        x + offsetX + pipWidth,
                        y + cardHeight - offsetY,
                        0,
                        -1,
                        -1,
                        0,
                        0,
                        color[1],
                        color[2],
                        color[3]
                    )
                end
                if mirrorX and mirrorY then
                    engine.image(
                        'blackjack_images/'..pipImage,
                        x + cardWidth - offsetX,
                        y + cardHeight - offsetY,
                        0,
                        -1,
                        -1,
                        0,
                        0,
                        color[1],
                        color[2],
                        color[3]
                    )
                end
            end

            local xLeft = 11
            local xMid = 21
            local yTop = 7
            local yThird = 19
            local yQtr = 23
            local yMid = 31

            if card.rank == 1 then
                drawPip(xMid, yMid)
            elseif card.rank == 2 then
                drawPip(xMid, yTop, false, true)
            elseif card.rank == 3 then
                drawPip(xMid, yTop, false, true)
                drawPip(xMid, yMid)
            elseif card.rank == 4 then
                drawPip(xLeft, yTop, true, true)
            elseif card.rank == 5 then
                drawPip(xLeft, yTop, true, true)
                drawPip(xMid, yMid)
            elseif card.rank == 6 then
                drawPip(xLeft, yTop, true, true)
                drawPip(xLeft, yMid, true)
            elseif card.rank == 7 then
                drawPip(xLeft, yTop, true, true)
                drawPip(xLeft, yMid, true)
                drawPip(xMid, yThird)
            elseif card.rank == 8 then
                drawPip(xLeft, yTop, true, true)
                drawPip(xLeft, yMid, true)
                drawPip(xMid, yThird, false, true)
            elseif card.rank == 9 then
                drawPip(xLeft, yTop, true, true)
                drawPip(xLeft, yQtr, true, true)
                drawPip(xMid, yMid)
            elseif card.rank == 10 then
                drawPip(xLeft, yTop, true, true)
                drawPip(xLeft, yQtr, true, true)
                drawPip(xMid, 16, false, true)
            end
        end
    end

    local cardSpacing = 60
    local marginX = 10

    for cardIndex, card in ipairs(dealerHand) do
        local dealerMarginY = 30
        if not roundOver and cardIndex == 1 then
            engine.image('blackjack_images/card_face_down.png', marginX, dealerMarginY)
        else
            drawCard(card, ((cardIndex - 1) * cardSpacing) + marginX, dealerMarginY)
        end
    end

    for cardIndex, card in ipairs(playerHand) do
        drawCard(card, ((cardIndex - 1) * cardSpacing) + marginX, 140)
    end

    if roundOver then
        engine.text('Total: '..getTotal(dealerHand), 'Vera.ttf', 12, marginX, 10)
    else
        engine.text('Total: ?', 'Vera.ttf', 12, marginX, 10)
    end

    engine.text('Total: '..getTotal(playerHand), 'Vera.ttf', 12, marginX, 120)

    if roundOver then
        local function hasHandWon(thisHand, otherHand)
            return getTotal(thisHand) <= 21
            and (
                getTotal(otherHand) > 21
                or getTotal(thisHand) > getTotal(otherHand)
            )
        end

        local function drawWinner(message)
            engine.text(message, 'Vera.ttf', 12, marginX, 268, 0, 0, 0)
        end

        if hasHandWon(playerHand, dealerHand) then
            drawWinner('Player wins')
        elseif hasHandWon(dealerHand, playerHand) then
            drawWinner('Dealer wins')
        else
            drawWinner('Draw')
        end
    end

    local function drawButton(text, buttonX, buttonWidth, textOffsetX)
        local buttonY = 230
        local buttonHeight = 25

        if engine.mouseX() >= buttonX
        and engine.mouseX() < buttonX + buttonWidth
        and engine.mouseY() >= buttonY
        and engine.mouseY() < buttonY + buttonHeight then
            engine.rectangle(buttonX, buttonY, buttonWidth, buttonHeight, 255, 200, 50)
        else
            engine.rectangle(buttonX, buttonY, buttonWidth, buttonHeight, 255, 120, 30)
        end
        engine.text(text, 'Vera.ttf', 12, buttonX + textOffsetX, buttonY + 6, 1, 1, 1)
    end

    if roundOver then
        drawButton('Play again', buttonPlayAgainX, buttonPlayAgainWidth, 24)
    else
        drawButton('Hit!', buttonHitX, buttonHitWidth, 16)
        drawButton('Stand', buttonStandX, buttonStandWidth, 8)
    end
end

function engine.up(input)
    if input == 'mouseLeft' then
        if not roundOver then
            if isMouseInButton(buttonHitX, buttonHitWidth) then
                takeCard(playerHand)
                if getTotal(playerHand) > 21 then
                    roundOver = true
                end
            elseif isMouseInButton(buttonStandX, buttonStandWidth) then
                roundOver = true
            end

            if roundOver then
                while getTotal(dealerHand) < 17 do
                    takeCard(dealerHand)
                end
            end
        elseif isMouseInButton(buttonPlayAgainX, buttonPlayAgainWidth) then
            reset()
        end
    end
end
