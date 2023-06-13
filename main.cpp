#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <QMessagebox>

struct Character {
    QString name;
    int strength;
    int speed;
    int agility;
};
class CharacterDetailsWindow : public QWidget {
    Q_OBJECT

public:
    CharacterDetailsWindow(QWidget *parent = nullptr) : QWidget(parent) {
        QVBoxLayout *layout = new QVBoxLayout(this);

        nameLabel = new QLabel(this);
        layout->addWidget(nameLabel);

        strengthLabel = new QLabel(this);
        layout->addWidget(strengthLabel);

        speedLabel = new QLabel(this);
        layout->addWidget(speedLabel);

        agilityLabel = new QLabel(this);
        layout->addWidget(agilityLabel);

        setLayout(layout);
    }

public slots:
    void displayCharacterDetails(const Character& character) {
        nameLabel->setText("Name: " + character.name);
        strengthLabel->setText("Strength: " + QString::number(character.strength));
        speedLabel->setText("Speed: " + QString::number(character.speed));
        agilityLabel->setText("Agility: " + QString::number(character.agility));

        show();
    }

private:
    QLabel *nameLabel;
    QLabel *strengthLabel;
    QLabel *speedLabel;
    QLabel *agilityLabel;

};

class CharacterCreationWidget : public QWidget {
    Q_OBJECT

public:
    CharacterCreationWidget(QWidget *parent = nullptr) : QWidget(parent), attributesChosen(false) {
        QVBoxLayout *layout = new QVBoxLayout(this);

        QLabel *nameLabel = new QLabel("Enter character name:", this);
        layout->addWidget(nameLabel);

        nameLineEdit = new QLineEdit(this);
        layout->addWidget(nameLineEdit);

        QLabel *strengthLabel = new QLabel("Enter strength points (0-10):", this);
        layout->addWidget(strengthLabel);

        strengthLineEdit = new QLineEdit(this);
        layout->addWidget(strengthLineEdit);

        QLabel *speedLabel = new QLabel("Enter speed points (0-10):", this);
        layout->addWidget(speedLabel);

        speedLineEdit = new QLineEdit(this);
        layout->addWidget(speedLineEdit);

        createButton = new QPushButton("Create", this);
        layout->addWidget(createButton);

        fightAgainButton = new QPushButton("Fight Again", this);
        fightAgainButton->setEnabled(false);
        layout->addWidget(fightAgainButton);

        resultLabel = new QLabel(this);
        layout->addWidget(resultLabel);

        setLayout(layout);

        connect(createButton, &QPushButton::clicked, this, &CharacterCreationWidget::createCharacter);
        connect(fightAgainButton, &QPushButton::clicked, this, &CharacterCreationWidget::fightAgain);
    }

signals:
    void characterCreated(Character character);
    void showCharacterDetails(const Character& character);

private slots:
    void createCharacter() {
        if (attributesChosen) {
            QMessageBox::information(this, "Error", "Attributes have already been chosen.");
            return;
        }

        Character character;
        character.name = nameLineEdit->text();
        character.strength = strengthLineEdit->text().toInt();
        character.speed = speedLineEdit->text().toInt();
        character.agility = 10 - character.strength - character.speed;

        if (character.strength + character.speed + character.agility > 10) {
            QMessageBox::critical(this, "Error", "The total attribute points cannot exceed 10.");
            return;
        }

        attributesChosen = true;
        setAttributeFieldsEnabled(false);
        player = character;
        createButton->setEnabled(false);

        emit characterCreated(player);
        emit showCharacterDetails(player);
    }

public slots:
    void displayFightResult(const QString& result) {
        resultLabel->setText(result);
        fightAgainButton->setEnabled(true);
    }

    void fightAgain() {
        resultLabel->clear();
        emit characterCreated(player);
    }

private:
    void setAttributeFieldsEnabled(bool enabled) {
        nameLineEdit->setEnabled(enabled);
        strengthLineEdit->setEnabled(enabled);
        speedLineEdit->setEnabled(enabled);
    }

    QLineEdit *nameLineEdit;
    QLineEdit *strengthLineEdit;
    QLineEdit *speedLineEdit;
    QLabel *resultLabel;
    QPushButton *createButton;
    QPushButton *fightAgainButton;
    bool attributesChosen;
    Character player;
};


QString fight(const Character& player, const Character& enemy) {
    QString result = "Player: " + player.name + " vs Enemy: " + enemy.name + "\n\n";

    int playerScore = player.strength + player.speed + player.agility;
    int enemyScore = enemy.strength + enemy.speed + enemy.agility;

    result += "Player Score: " + QString::number(playerScore) + "\n";
    result += "Enemy Score: " + QString::number(enemyScore) + "\n\n";

    if (playerScore > enemyScore) {
        result += "Player wins!";
    } else if (playerScore < enemyScore) {
        result += "Enemy wins!";
    } else {
        result += "It's a draw!";
    }

    return result;
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    srand(time(NULL));

    Character player;

    CharacterCreationWidget creationWidget;
    QObject::connect(&creationWidget, &CharacterCreationWidget::characterCreated, [&](Character character) {
        player = character;

        Character enemy;
        enemy.name = "Enemy";
        enemy.strength = rand() % 11;
        enemy.speed = rand() % 11;
        enemy.agility = rand() % 11;

        QString result = fight(player, enemy);
        creationWidget.displayFightResult(result);
    });
    CharacterDetailsWindow detailsWindow;
    QObject::connect(&creationWidget, &CharacterCreationWidget::showCharacterDetails, &detailsWindow, &CharacterDetailsWindow::displayCharacterDetails);
    creationWidget.show();

    return app.exec();
}

#include "main.moc"
