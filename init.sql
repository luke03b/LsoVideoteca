-- Database initialization script for the video rental system

-- Create tables

-- Film table
CREATE TABLE IF NOT EXISTS film (
    id SERIAL PRIMARY KEY,
    titolo VARCHAR(255) NOT NULL,
    genere VARCHAR(100) NOT NULL,
    copie_totali INTEGER NOT NULL,
    copie_disponibili INTEGER NOT NULL,
    noleggi_totali INTEGER NOT NULL DEFAULT 0
);

-- User table
CREATE TABLE IF NOT EXISTS utenti (
    id SERIAL PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    password VARCHAR(50) NOT NULL
);

-- Notification table
CREATE TABLE IF NOT EXISTS notifiche (
    id SERIAL PRIMARY KEY,
    id_utente INTEGER NOT NULL,
    messaggio TEXT NOT NULL,
    data_invio TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    visualizzato BOOLEAN NOT NULL DEFAULT FALSE,
    FOREIGN KEY (id_utente) REFERENCES utenti(id) ON DELETE CASCADE
);

-- Rental table
CREATE TABLE IF NOT EXISTS noleggi (
    id SERIAL PRIMARY KEY,
    id_utente INTEGER NOT NULL,
    id_film INTEGER NOT NULL,
    data_noleggio TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    data_restituzione TIMESTAMP NOT NULL,
    restituito BOOLEAN NOT NULL DEFAULT FALSE,
    FOREIGN KEY (id_utente) REFERENCES utenti(id) ON DELETE CASCADE,
    FOREIGN KEY (id_film) REFERENCES film(id) ON DELETE CASCADE
);

-- Insert some sample films
INSERT INTO film (titolo, genere, copie_totali, copie_disponibili, noleggi_totali) VALUES
('Il Padrino', 'Drammatico', 5, 5, 0),
('Pulp Fiction', 'Thriller', 3, 3, 0),
('Inception', 'Fantascienza', 4, 4, 0),
('The Dark Knight', 'Azione', 6, 6, 0),
('Shawshank Redemption', 'Drammatico', 3, 3, 0),
('Forrest Gump', 'Drammatico', 4, 4, 0),
('Matrix', 'Fantascienza', 5, 5, 0),
('Il Signore degli Anelli', 'Fantasy', 3, 3, 0),
('Titanic', 'Romantico', 2, 2, 0),
('Jurassic Park', 'Avventura', 4, 4, 0);

-- Insert a test user
INSERT INTO utenti (username, password) VALUES ('test', 'password');
