#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <sstream>

// Function declaration
void updateBranches(int seed);

const int NUM_BRANCHES = 6;
sf::Sprite branches[NUM_BRANCHES];

// Where is the player/branch?
// Left or Right
enum class side { LEFT, RIGHT, NONE };
side branchPositions[NUM_BRANCHES];

int main(int, char const**)
{
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Timber");
    
    //Load Font
    sf::Font font;
    font.loadFromFile("fonts/KOMIKAP_.ttf");
    
    //Init texts to show on screen
    sf::Text messageText;
    sf::Text scoreText;
    messageText.setFont(font);
    scoreText.setFont(font);
    
    messageText.setString("Press Enter to Start!!");
    messageText.setCharacterSize(75);
    messageText.setFillColor(sf::Color::White);
    
    scoreText.setString("Score = 0");
    scoreText.setCharacterSize(100);
    scoreText.setFillColor(sf::Color::White);
    
    // Position the text
    sf::FloatRect textRect = messageText.getLocalBounds();
    
    //Setting origin of messageText to be in the middle of the text instead of left corner
    messageText.setOrigin(textRect.left +
                          textRect.width / 2.0f,
                          textRect.top +
                          textRect.height / 2.0f);
    
    messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
    
    scoreText.setPosition(20, 20);
    
    //Background
    sf::Texture bckgTexture;
    bckgTexture.loadFromFile("graphics/background.png");
    sf::Sprite bckg;
    bckg.setTexture(bckgTexture);
    
    //Tree
    sf::Texture treeTexture;
    treeTexture.loadFromFile("graphics/tree.png");
    sf::Sprite tree;
    tree.setTexture(treeTexture);
    tree.setPosition(810,0);
    
    //Bee
    sf::Texture beeTexture;
    beeTexture.loadFromFile("graphics/bee.png");
    sf::Sprite bee;
    bee.setTexture(beeTexture);
    bee.setPosition(0,800);
    bool beeActive = false;
    float beeSpeed = 0.0f;
    
    //Clouds
    sf::Texture cloudTexture;
    cloudTexture.loadFromFile("graphics/cloud.png");
    
    const int NUM_CLOUDS = 3;
    sf::Sprite clouds[NUM_CLOUDS];
    bool activeClouds[NUM_CLOUDS];
    int cloudsSpeed[NUM_CLOUDS];
    
    // Set clouds in an array
    for (int i = 0; i < NUM_CLOUDS; i++)
    {
        clouds[i].setTexture(cloudTexture);
        clouds[i].setPosition(0, i * 250);
        activeClouds[i] = false;
        cloudsSpeed[i] = 0.0f;
    }
    
    //Time bar
    sf::RectangleShape timeBar;
    float timeBarStartWidth = 400;
    float timeBarHeight = 80;
    timeBar.setSize(sf::Vector2f(timeBarStartWidth, timeBarHeight));
    timeBar.setFillColor(sf::Color::Red);
    timeBar.setPosition((1920 / 2) - timeBarStartWidth / 2, 980);
    
    sf::Time gameTimeTotal;
    float timeRemaining = 6.0f;
    float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;
    
    // Prepare 6 branches
    sf::Texture textureBranch;
    textureBranch.loadFromFile("graphics/branch.png");
    
    // Set the texture for each branch sprite
    for (int i = 0; i < NUM_BRANCHES; i++)
    {
        branches[i].setTexture(textureBranch);
        branches[i].setPosition(-2000, -2000);
        // Set the sprite's origin to dead center
        // We can then spin it round without changing its position
        branches[i].setOrigin(220, 20);
    }
    
    // Prepare the player
    sf::Texture texturePlayer;
    texturePlayer.loadFromFile("graphics/player.png");
    sf::Sprite spritePlayer;
    spritePlayer.setTexture(texturePlayer);
    spritePlayer.setPosition(580, 720);
    
    // The player starts on the left
    side playerSide = side::LEFT;
    
    // Prepare the gravestone
    sf::Texture textureRIP;
    textureRIP.loadFromFile("graphics/rip.png");
    sf::Sprite spriteRIP;
    spriteRIP.setTexture(textureRIP);
    spriteRIP.setPosition(675, 2000);
    
    // Prepare the axe
    sf::Texture textureAxe;
    textureAxe.loadFromFile("graphics/axe.png");
    sf::Sprite spriteAxe;
    spriteAxe.setTexture(textureAxe);
    spriteAxe.setPosition(2000, 830);
    
    // Line the axe up with the tree
    const float AXE_POSITION_LEFT = 700;
    const float AXE_POSITION_RIGHT = 1075;
    
    // Prepare the flying log
    sf::Texture textureLog;
    textureLog.loadFromFile("graphics/log.png");
    sf::Sprite spriteLog;
    spriteLog.setTexture(textureLog);
    spriteLog.setPosition(810, 720);
    
    // Some other useful log related variables
    bool logActive = false;
    float logSpeedX = 1000;
    float logSpeedY = -1500;
    
    // Prepare the sound
    sf::SoundBuffer chopBuffer;
    chopBuffer.loadFromFile("sound/chop.wav");
    sf::Sound chop;
    chop.setBuffer(chopBuffer);
    
    sf::SoundBuffer deathBuffer;
    deathBuffer.loadFromFile("sound/death.wav");
    sf::Sound death;
    death.setBuffer(deathBuffer);
    
    // Out of time
    sf::SoundBuffer ootBuffer;
    ootBuffer.loadFromFile("sound/out_of_time.wav");
    sf::Sound outOfTime;
    outOfTime.setBuffer(ootBuffer);
    
    //Initial vars
    bool isPaused = true;
    bool acceptInput = true;
    int score = 0;
    timeRemaining = 5;
    
    //Variables to control time
    sf::Clock clock;
    
    // Start the game loop
    while (window.isOpen())
    {
        //1. Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            
            if (event.type == sf::Event::KeyReleased && !isPaused)
            {
                // Listen for key presses again
                acceptInput = true;
                
                // hide the axe
                spriteAxe.setPosition(2000,
                                      spriteAxe.getPosition().y);
            }
            
            if (event.type == sf::Event::KeyPressed)
            {
                switch (event.key.code) {
                    case sf::Keyboard::Escape:
                        window.close();
                        break;
                    case sf::Keyboard::Return:
                        if (isPaused)
                        {
                            isPaused = false;
                            score = 0;
                            timeRemaining = 5;
                            
                            // Make all the branches disappear
                            for (int i = 1; i < NUM_BRANCHES; i++)
                            {
                                branchPositions[i] = side::NONE;
                            }
                            
                            // Make sure the gravestone is hidden
                            spriteRIP.setPosition(675, 2000);
                            
                            // Move the player into position
                            spritePlayer.setPosition(580, 720);
                            
                            acceptInput = true;
                            
                            clock.restart();
                        }
                        break;
                    default:
                        break;
                }
            }
            
            if (acceptInput)
            {
                if (event.type == sf::Event::KeyPressed)
                {
                    switch (event.key.code) {
                        case sf::Keyboard::Right:
                            // Make sure the player is on the right
                            playerSide = side::RIGHT;
                            
                            score ++;
                            
                            // Add to the amount of time remaining
                            timeRemaining += (2 / score) + .15;
                            
                            spriteAxe.setPosition(AXE_POSITION_RIGHT,
                                                  spriteAxe.getPosition().y);
                            
                            spritePlayer.setPosition(1200, 720);
                            
                            // update the branches
                            updateBranches(score);
                            
                            // set the log flying to the left
                            spriteLog.setPosition(810, 720);
                            logSpeedX = -5000;
                            logActive = true;
                            
                            acceptInput = false;
                            
                            // Play a chop sound
                            chop.play();
                            break;
                        case sf::Keyboard::Left:
                            // Make sure the player is on the left
                            playerSide = side::LEFT;
                            
                            score++;
                            
                            // Add to the amount of time remaining
                            timeRemaining += (2 / score) + .15;
                            
                            spriteAxe.setPosition(AXE_POSITION_LEFT,
                                                  spriteAxe.getPosition().y);
                            
                            spritePlayer.setPosition(580, 720);
                            
                            // update the branches
                            updateBranches(score);
                            
                            // set the log flying
                            spriteLog.setPosition(810, 720);
                            logSpeedX = 5000;
                            logActive = true;
                            
                            acceptInput = false;
                            
                            // Play a chop sound
                            chop.play();
                            break;
                        default:
                            break;
                    }
                }
            }
        }
        
        
        if (!isPaused) {
            
            //2. Update scene
            
            //Saving amount of time since last restart of the clock
            sf::Time dt = clock.restart();
            
            //Moving the bee
            if (!beeActive)
            {
                srand((int)time(0) * 10);
                beeSpeed = (rand() % 200) + 200;
                
                srand((int)time(0) * 10);
                float height = (rand() % 500) + 500;
                bee.setPosition(2000, height);
                beeActive = true;
            }
            else
            {
                bee.setPosition(bee.getPosition().x - (beeSpeed * dt.asSeconds()), bee.getPosition().y);
                
                if (bee.getPosition().x < -100)
                {
                    beeActive = false;
                }
            }
            
            for (int i = 0; i < NUM_CLOUDS; ++i)
            {
                if (!activeClouds[i])
                {
                    srand((int)time(0) * (10 * i + 10));
                    cloudsSpeed[i] = (rand() % 200);
                    
                    srand((int)time(0) * 10);
                    float height = (rand() % 100) + 75 * i;
                    clouds[i].setPosition(-200, height);
                    activeClouds[i] = true;
                } else
                {
                    clouds[i].setPosition( clouds[i].getPosition().x + (cloudsSpeed[i] * dt.asSeconds()) , clouds[i].getPosition().y);
                    if (clouds[i].getPosition().x > 1920)
                    {
                        activeClouds[i] = false;
                    }
                }
            }
            
            //Updating score
            std::stringstream ss;
            ss << "Score = " << score;
            scoreText.setString(ss.str());
            
            //Updating bar
            // Subtract from the amount of time remaining
            timeRemaining -= dt.asSeconds();
            // size up the time bar
            timeBar.setSize(sf::Vector2f(timeBarWidthPerSecond *
                                         timeRemaining, timeBarHeight));
            
            if (timeRemaining <= 0.0f)
            {
                // Pause the game
                isPaused = true;
                
                // Change the message shown to the player
                messageText.setString("Out of time!!");
                
                //Reposition the text based on its new size
                sf::FloatRect textRect = messageText.getLocalBounds();
                messageText.setOrigin(textRect.left +
                                      textRect.width / 2.0f,
                                      textRect.top +
                                      textRect.height / 2.0f);
                
                messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
                // Play the out of time sound
                outOfTime.play();
            }
            
            // update the branch sprites
            for (int i = 0; i < NUM_BRANCHES; i++)
            {
                float height = i * 150;
                if (branchPositions[i] == side::LEFT)
                {
                    // Move the sprite to the left side
                    branches[i].setPosition(610, height);
                    
                    // Flip the sprite round the other way
                    branches[i].setRotation(180);
                }
                
                else if (branchPositions[i] == side::RIGHT)
                {
                    // Move the sprite to the right side
                    branches[i].setPosition(1330, height);
                    
                    // Set the sprite rotation to normal
                    branches[i].setRotation(0);
                }
                else
                {
                    // Hide the branch
                    branches[i].setPosition(3000, height);
                }
            }
            
            //Update flying log
            if (logActive)
            {
                spriteLog.setPosition(
                                      spriteLog.getPosition().x +
                                      (logSpeedX * dt.asSeconds()),
                                      
                                      spriteLog.getPosition().y +
                                      (logSpeedY * dt.asSeconds()));
                
                // Has the log reached the right hand edge?
                if (spriteLog.getPosition().x < -100 ||
                    spriteLog.getPosition().x > 2000)
                {
                    // Set it up ready to be a whole new log next frame
                    logActive = false;
                    spriteLog.setPosition(810, 720);
                }
            }
            
            // Has the player been squished by a branch?
            if (branchPositions[5] == playerSide)
            {
                // death
                isPaused = true;
                acceptInput = false;
                
                // Draw the gravestone
                if (playerSide == side::LEFT)
                {
                    spriteRIP.setPosition(525, 760);
                } else
                {
                    spriteRIP.setPosition(1200, 760);
                }
                
                // hide the player
                spritePlayer.setPosition(2000, 660);
                
                // Change the text of the message
                messageText.setString("SQUISHED!!");
                
                // Center it on the screen
                sf::FloatRect textRect = messageText.getLocalBounds();
                
                messageText.setOrigin(textRect.left +
                                      textRect.width / 2.0f,
                                      textRect.top + textRect.height / 2.0f);
                
                messageText.setPosition(1920 / 2.0f,
                                        1080 / 2.0f);
                
                //hide axe
                spriteAxe.setPosition(2000,
                                      spriteAxe.getPosition().y);
                
                // Play the death sound
                death.play();
                
            }
        }
        
        
        //3. Draw scene
        
        // Clear screen
        window.clear();
        
        // Draw the bckg
        window.draw(bckg);
        
        
        //Draw clouds before the tree to draw them behind it
        for (int i = 0; i <NUM_CLOUDS; ++i)
        {
            window.draw(clouds[i]);
        }
        
        //Draw the tree
        window.draw(tree);
        
        // Draw the player
        window.draw(spritePlayer);
        
        // Draw the axe
        window.draw(spriteAxe);
        
        // Draraw the flying log
        window.draw(spriteLog);
        
        // Draw the gravestone
        window.draw(spriteRIP);
        
        //Draw bee after tree to put it above the tree!
        window.draw(bee);
        
        // Draw the score
        window.draw(scoreText);
        
        // Draw the timebar
        window.draw(timeBar);
        
        // Draw the branches
        for (int i = 0; i < NUM_BRANCHES; i++)
        {
            window.draw(branches[i]);
        }
        
        if (isPaused)
        {
            // Draw our message
            window.draw(messageText);
        }
        
        
        // Update the window
        window.display();
    }
    
    return EXIT_SUCCESS;
}

// Function definition
void updateBranches(int seed)
{
    // Move all the branches down one place
    for (int j = NUM_BRANCHES-1; j > 0; j--)
    {
        branchPositions[j] = branchPositions[j - 1];
    }
    
    // Spawn a new branch at position 0
    // LEFT, RIGHT or NONE
    srand((int)time(0)+seed);
    int r = (rand() % 5);
    switch (r)
    {
        case 0:
            branchPositions[0] = side::LEFT;
            break;
            
        case 1:
            branchPositions[0] = side::RIGHT;
            break;
            
        default:
            branchPositions[0] = side::NONE;
            break;
    }
}
