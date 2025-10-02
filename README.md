

-----

# 🚀 QTURSO: Il Driver Qt per Turso (libSQL)

Un plugin **`QSqlDriver`** per connettere le applicazioni Qt/C++ al database distribuito Turso (basato su libSQL).

-----

## 💡 1. Introduzione e Obiettivi del Progetto

Il modulo **Qt SQL** offre un'interfaccia database-agnostica (`QSqlDatabase`, `QSqlQuery`). Tuttavia, per connettersi a sistemi come **Turso**, è necessario un driver dedicato.

**QTURSO** colma questo divario, estendendo le capacità di Qt SQL:

  * Consente alle applicazioni Qt di accedere a un **database distribuito** e ad **alta disponibilità**.
  * Sfrutta i vantaggi di **Turso** ( SQLite riscritto in rust).

-----

## 🛠️ 2. Architettura e Implementazione

Il driver QTURSO è un plugin che implementa i seguenti componenti di Qt SQL:

### A. QTURSO\_Driver (`QSqlDriver`)
.

### B. QTURSO\_Result (`QSqlResult`)

Si occupa dell'esecuzione delle istruzioni SQL e della gestione dei set di risultati.

  * **Esecuzione Query:** Invia l'istruzione SQL al server Turso.
  * **Mappatura dei Dati:** Converte la risposta JSON/Protocollo libSQL (che contiene i dati) nei tipi nativi **`QVariant`** di Qt.

-----

## 🔌 3. Guida Rapida all'Uso

### 3.1. Aggiungere il Driver

Dopo aver compilato e installato il plugin `qturso.so`/`.dll`, si utilizza come un qualsiasi driver Qt:

```cpp
#include <QSqlDatabase>
#include <QDebug>

// Aggiungere il database specificando il nome del driver QTURSO
QSqlDatabase db = QSqlDatabase::addDatabase("QTURSO"); 
```

### 3.2. Configurare e Aprire la Connessione

È necessario specificare l'URL del tuo database Turso e il relativo Token API:

```cpp
    db.setDatabaseName("data/db.sqlite");
if (!db.open()) {
    qCritical() << "ERRORE: Impossibile connettersi a Turso:" << db.lastError().text();
} else {
    qInfo() << "SUCCESS: Connessione QTURSO stabilita.";
}
```

### 3.3. Esecuzione di una Query

Le query vengono eseguite in modo standard con `QSqlQuery`:

```cpp
QSqlQuery query(db);

if (query.exec("SELECT id, name FROM users WHERE active = TRUE")) {
    while (query.next()) {
        int id = query.value("id").toInt();
        QString name = query.value("name").toString();
        qDebug() << "Utente ID:" << id << "Nome:" << name;
    }
} else {
    qWarning() << "Errore Query:" << query.lastError().text();
}
```

-----

## 🛣️ 4. Stato di Sviluppo e Roadmap

### ✅ Funzionalità Attuali

  * Connessione e Disconnessione.
  * Esecuzione di query DML/DDL semplici (SELECT, INSERT, CREATE TABLE, ecc.).
  * Mappatura dei tipi di dati di base.

### 🔜 Prossimi Passi (Roadmap)

1.  **Prepared Statements Avanzati:** Implementazione ottimizzata dei binding per `QSqlQuery::prepare()`.
2.  **Transazioni (Batching):** Pieno supporto per `QSqlDriver::beginTransaction()`, `commit()`, e `rollback()`.
3.  **Gestione degli Errori:** Mappatura più granulare degli errori Turso/libSQL con `QSqlError`.

-----
### Nota ###
Il driver non è per test.


