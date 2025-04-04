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
    data_restituzione TIMESTAMP NOT NULL DEFAULT (CURRENT_TIMESTAMP + INTERVAL '7 days'),
    restituito BOOLEAN NOT NULL DEFAULT FALSE,
    FOREIGN KEY (id_utente) REFERENCES utenti(id) ON DELETE CASCADE,
    FOREIGN KEY (id_film) REFERENCES film(id) ON DELETE CASCADE
);

-- Aggiungi un indice parziale per garantire che un utente possa avere solo un noleggio attivo per film
CREATE UNIQUE INDEX idx_unique_noleggio_attivo 
ON noleggi (id_utente, id_film) 
WHERE restituito = FALSE;


-- Funzione per gestire il noleggio del film con limite di 5 noleggi attivi per utente
CREATE OR REPLACE FUNCTION noleggia_film(
    p_id_utente INTEGER,
    p_id_film INTEGER
) RETURNS BOOLEAN AS $$
DECLARE
    v_copie_disponibili INTEGER;
    v_noleggi_attivi INTEGER;
BEGIN
    -- Verifica se il film ha copie disponibili
    SELECT copie_disponibili INTO v_copie_disponibili
    FROM film
    WHERE id = p_id_film;

    -- Se non ci sono copie disponibili, ritorna false
    IF v_copie_disponibili IS NULL OR v_copie_disponibili <= 0 THEN
        RETURN FALSE;
    END IF;

    -- Conta quanti noleggi attivi ha l'utente
    SELECT COUNT(*) INTO v_noleggi_attivi
    FROM noleggi
    WHERE id_utente = p_id_utente AND restituito = FALSE;

    -- Se l'utente ha già 5 noleggi attivi, ritorna false
    IF v_noleggi_attivi >= 5 THEN
        RETURN FALSE;
    END IF;

    -- Inizia la transazione per garantire l'atomicità dell'operazione
    BEGIN
        -- Aggiorna la tabella film
        UPDATE film
        SET copie_disponibili = copie_disponibili - 1,
            noleggi_totali = noleggi_totali + 1
        WHERE id = p_id_film;

        -- Inserisci il nuovo noleggio
        INSERT INTO noleggi (
            id_utente,
            id_film
        ) VALUES (
            p_id_utente,
            p_id_film
        );

        -- Se tutto è andato bene, conferma la transazione
        RETURN TRUE;
    EXCEPTION
        WHEN unique_violation THEN
            -- L'utente ha già un noleggio attivo per questo film
            ROLLBACK;
            RETURN FALSE;
        WHEN OTHERS THEN
            -- In caso di altri errori
            ROLLBACK;
            RAISE;
    END;
END;
$$ LANGUAGE plpgsql;


-- Funzione per gestire la restituzione di un film
CREATE OR REPLACE FUNCTION restituisci_film(
    p_id_utente INTEGER,
    p_id_film INTEGER
) RETURNS BOOLEAN AS $$
DECLARE
    v_noleggio_id INTEGER;
BEGIN
    -- Verifica se esiste un noleggio attivo per questo utente e film
    SELECT id INTO v_noleggio_id
    FROM noleggi
    WHERE id_utente = p_id_utente 
      AND id_film = p_id_film
      AND restituito = FALSE
    LIMIT 1;

    -- Se non esiste un noleggio attivo, ritorna false
    IF v_noleggio_id IS NULL THEN
        RETURN FALSE;
    END IF;

    -- Inizia la transazione per garantire l'atomicità dell'operazione
    BEGIN
        -- Aggiorna la tabella noleggi impostando il film come restituito
        UPDATE noleggi
        SET restituito = TRUE,
            data_restituzione = CURRENT_TIMESTAMP  -- Opzionale: aggiorna la data di restituzione con la data effettiva
        WHERE id = v_noleggio_id;

        -- Aggiorna la tabella film aumentando le copie disponibili
        UPDATE film
        SET copie_disponibili = copie_disponibili + 1
        WHERE id = p_id_film;

        -- Se tutto è andato bene, conferma la transazione
        RETURN TRUE;
    EXCEPTION
        WHEN OTHERS THEN
            -- In caso di errori
            ROLLBACK;
            RAISE;
    END;
END;
$$ LANGUAGE plpgsql;


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
