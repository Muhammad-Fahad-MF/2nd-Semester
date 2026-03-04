/*
 * ============================================================
 *  PakWheels-Style Car Marketplace System
 *  Subject : OOP - CS1004  |  Spring 2026
 *
 *  Design Decisions Summary
 *  ─────────────────────────
 *  COMPOSITION  : CarListing owns Car (car dies with listing)
 *                 User owns Address (address dies with user)
 *                 Conversation owns Message[] (msgs die with conv)
 *
 *  AGGREGATION  : Buyer holds const CarListing* favorites[]
 *                   → listing lives in Seller, Buyer only points
 *                 Marketplace holds vector<Seller*> / Buyer* / Admin*
 *                   → deleting Marketplace calls delete on each,
 *                     but the objects themselves were heap-allocated
 *                     independently
 *
 *  INHERITANCE  : Buyer, Seller, Admin all extend User
 *
 *  STATIC       : Car::cars_count, User::userCount,
 *                 CarListing::totalListings, Message::messageCount,
 *                 Admin::totalAdmins, Marketplace::sessionCount
 *
 *  CONST MEMBERS: Car::id, User::userId, Message::messageId,
 *                 Address::country, Seller::MAX_LISTINGS,
 *                 Marketplace::PLATFORM_NAME
 *
 *  ARRAYS OF OBJ: Seller::listings[10], Conversation::messages[50],
 *                 Buyer::conversations[10]
 * ============================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <ctime>

using namespace std;

// ============================================================
//  ENUMS  – defined globally so every class can use them
// ============================================================

enum class Category   { SEDAN, SUV, VAN, HATCHBACK, TRUCK   };
enum class FuelType   { PETROL, DIESEL, HYBRID, ELECTRIC     };
enum class ListingStatus { PENDING, APPROVED, SOLD, REMOVED  };

// ============================================================
//  UTILITY HELPERS
// ============================================================

string getCurrentDate() {
    time_t now  = time(0);
    struct tm* t = localtime(&now);
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d", t);
    return string(buf);
}

string categoryToStr(Category c) {
    switch(c) {
        case Category::SEDAN:    return "Sedan";
        case Category::SUV:      return "SUV";
        case Category::VAN:      return "Van";
        case Category::HATCHBACK:return "Hatchback";
        case Category::TRUCK:    return "Truck";
        default:                 return "Unknown";
    }
}

string fuelToStr(FuelType f) {
    switch(f) {
        case FuelType::PETROL:   return "Petrol";
        case FuelType::DIESEL:   return "Diesel";
        case FuelType::HYBRID:   return "Hybrid";
        case FuelType::ELECTRIC: return "Electric";
        default:                 return "Unknown";
    }
}

string statusToStr(ListingStatus s) {
    switch(s) {
        case ListingStatus::PENDING:  return "Pending";
        case ListingStatus::APPROVED: return "Approved";
        case ListingStatus::SOLD:     return "Sold";
        case ListingStatus::REMOVED:  return "Removed";
        default:                      return "Unknown";
    }
}

void printLine() { cout << string(44, '-') << endl; }

// ============================================================
//  FORWARD DECLARATIONS
//  Needed because Buyer references CarListing before it is defined
// ============================================================
class CarListing;

// ============================================================
//  CLASS 1 : Address
//  Purpose  : Represents a physical location.
//  Relation : Embedded inside User → COMPOSITION
//             (address has no meaning without its User)
// ============================================================
class Address {
private:
    // CONST MEMBER declared first so initialiser list order matches declaration order
    const string country;   // CONST MEMBER – always "Pakistan" in this system
    string city;
    string area;
    string province;
    int postalCode;

public:
    /*
     * Default constructor
     * Reason: User has a default constructor (needed for pointer
     * initialisation), so Address must also have one.
     */
    Address()
        : country("Pakistan"), city(""), area(""), province(""), postalCode(0) {}

    /*
     * Parameterised constructor
     * Reason: Used whenever a real address is built with full data.
     */
    Address(string city, string area, string province, int postalCode)
        : country("Pakistan"), city(city), area(area),
          province(province), postalCode(postalCode) {}

    /*
     * Copy constructor
     * Reason: When a User is copied, its embedded Address must also
     * be deep-copied.  Because country is const we must re-initialise
     * it explicitly in the initialiser list.
     */
    Address(const Address& o)
        : country("Pakistan"), city(o.city), area(o.area),
          province(o.province), postalCode(o.postalCode) {}

    // Copy-assignment: country is const so we skip it (it is always "Pakistan")
    Address& operator=(const Address& o) {
        if (this != &o) {
            city       = o.city;
            area       = o.area;
            province   = o.province;
            postalCode = o.postalCode;
        }
        return *this;
    }

    // --- Getters ---
    string getCity()       const { return city;       }
    string getArea()       const { return area;       }
    string getProvince()   const { return province;   }
    string getCountry()    const { return country;    }
    int    getPostalCode() const { return postalCode; }

    // --- Setters ---
    void setCity(string c)     { city       = c; }
    void setArea(string a)     { area       = a; }
    void setProvince(string p) { province   = p; }
    void setPostalCode(int pc) { postalCode = pc; }

    // Member function 1: print formatted address
    void display() const {
        cout << area << ", " << city << ", " << province
             << ", " << country;
        if (postalCode) cout << " - " << postalCode;
    }

    // Member function 2: validate 5-digit postal code
    bool isValidPostalCode() const {
        return postalCode >= 10000 && postalCode <= 99999;
    }

    // Member function 3: compare cities
    bool isSameCity(const Address& other) const {
        return city == other.city;
    }

    // Member function 4: CONST FUNCTION – returns formatted string
    string toString() const {
        return area + ", " + city + ", " + province + ", " + country;
    }
};

// ============================================================
//  CLASS 2 : Car
//  Purpose  : Core vehicle data.
//  Relation : Embedded inside CarListing → COMPOSITION
//             (car details have no meaning without a listing)
// ============================================================
class Car {
private:
    const int id;           // CONST MEMBER – assigned once, never changes
    static int cars_count;  // STATIC MEMBER – shared counter across ALL Car instances
                            // Why static? We need one global counter regardless
                            // of which object we query.
    string make;
    string model;
    int    year;
    int    mileage;
    int    engineCC;
    string color;
    Category category;
    FuelType fuelType;

public:
    /*
     * Default constructor
     * Reason: CarListing has an array of Car slots that must be
     * initialised before real data arrives.
     */
    Car()
        : id(++cars_count), make(""), model(""), year(2020),
          mileage(0), engineCC(1000), color("White"),
          category(Category::SEDAN), fuelType(FuelType::PETROL) {}

    /*
     * Parameterised constructor
     * Reason: Called with real data when a seller fills in the form.
     */
    Car(string make, string model, int year, int mileage,
        int engineCC, string color, Category cat, FuelType fuel)
        : id(++cars_count), make(make), model(model), year(year),
          mileage(mileage), engineCC(engineCC), color(color),
          category(cat), fuelType(fuel) {}

    /*
     * Copy constructor
     * Reason: When a CarListing is copied the embedded Car must be
     * duplicated cleanly.  A new id is assigned so each Car object
     * has a unique id even if copied from another.
     */
    Car(const Car& o)
        : id(++cars_count), make(o.make), model(o.model), year(o.year),
          mileage(o.mileage), engineCC(o.engineCC), color(o.color),
          category(o.category), fuelType(o.fuelType) {}

    // Copy-assignment operator (needed because id is const)
    // We keep the original id so the car's identity doesn't change on assignment
    Car& operator=(const Car& o) {
        if (this != &o) {
            make      = o.make;
            model     = o.model;
            year      = o.year;
            mileage   = o.mileage;
            engineCC  = o.engineCC;
            color     = o.color;
            category  = o.category;
            fuelType  = o.fuelType;
            // id is const – intentionally not copied
        }
        return *this;
    }

    // --- Getters ---
    int      getId()       const { return id;       }
    string   getMake()     const { return make;     }
    string   getModel()    const { return model;    }
    int      getYear()     const { return year;     }
    int      getMileage()  const { return mileage;  }
    int      getEngineCC() const { return engineCC; }
    string   getColor()    const { return color;    }
    Category getCategory() const { return category; }
    FuelType getFuelType() const { return fuelType; }

    // --- Setters (no setter for const id) ---
    void setMake(string m)     { make     = m; }
    void setModel(string m)    { model    = m; }
    void setYear(int y)        { year     = y; }
    void setMileage(int m)     { mileage  = m; }
    void setEngineCC(int e)    { engineCC = e; }
    void setColor(string c)    { color    = c; }
    void setCategory(Category c){ category = c; }
    void setFuelType(FuelType f){ fuelType = f; }

    // Member function 1: CONST FUNCTION – display all info without modifying object
    void display() const {
        cout << "  Make      : " << make
             << "  |  Model   : " << model     << endl;
        cout << "  Year      : " << year
             << "  |  Mileage : " << mileage   << " km" << endl;
        cout << "  Engine    : " << engineCC   << " cc"
             << "  |  Color   : " << color     << endl;
        cout << "  Category  : " << categoryToStr(category)
             << "  |  Fuel    : " << fuelToStr(fuelType) << endl;
    }

    // Member function 2: check if car runs on electricity
    bool isElectric() const {
        return fuelType == FuelType::ELECTRIC;
    }

    // Member function 3: age of car in years
    int getAgeInYears() const {
        return 2026 - year;
    }

    // Member function 4: check if this car matches given filter criteria
    bool matchesFilter(const string& filterMake, int minYear, int maxYear,
                       int maxMileage) const {
        if (!filterMake.empty() && make != filterMake) return false;
        if (year < minYear || year > maxYear)           return false;
        if (mileage > maxMileage)                       return false;
        return true;
    }

    // STATIC FUNCTION – reports class-level data, doesn't need an instance
    static int getCarsCount() { return cars_count; }
};

int Car::cars_count = 0;   // Static member initialised outside class

// ============================================================
//  CLASS 3 : Message
//  Purpose  : A single chat message between buyer and seller.
//  Relation : Embedded inside Conversation → COMPOSITION
//             (a message can't exist outside its conversation)
// ============================================================
class Message {
private:
    const int messageId;        // CONST MEMBER – ID never changes
    static int messageCount;    // STATIC – counts every message ever created
                                // Why static? System-wide statistic, not per-object.
    int    senderId;
    int    receiverId;
    string content;
    string timestamp;
    bool   isRead;

public:
    /*
     * Default constructor
     * Reason: Conversation stores Message objects in a fixed-size array.
     * Array elements must be default-constructible.
     */
    Message()
        : messageId(++messageCount), senderId(0), receiverId(0),
          content(""), timestamp(getCurrentDate()), isRead(false) {}

    /*
     * Parameterised constructor
     * Reason: Used when a buyer actually sends a message with real content.
     */
    Message(int senderId, int receiverId, const string& content)
        : messageId(++messageCount), senderId(senderId), receiverId(receiverId),
          content(content), timestamp(getCurrentDate()), isRead(false) {}

    /*
     * Copy constructor
     * Reason: When a Message is copied into the array it needs its own id.
     */
    Message(const Message& o)
        : messageId(++messageCount), senderId(o.senderId),
          receiverId(o.receiverId), content(o.content),
          timestamp(o.timestamp), isRead(o.isRead) {}

    // Copy-assignment (keep existing id so the stored message retains identity)
    Message& operator=(const Message& o) {
        if (this != &o) {
            senderId   = o.senderId;
            receiverId = o.receiverId;
            content    = o.content;
            timestamp  = o.timestamp;
            isRead     = o.isRead;
        }
        return *this;
    }

    // --- Getters ---
    int    getMessageId()  const { return messageId;  }
    int    getSenderId()   const { return senderId;   }
    int    getReceiverId() const { return receiverId; }
    string getContent()    const { return content;    }
    string getTimestamp()  const { return timestamp;  }
    bool   getIsRead()     const { return isRead;     }

    // --- Setters ---
    void setSenderId(int id)          { senderId   = id; }
    void setReceiverId(int id)        { receiverId = id; }
    void setContent(const string& c)  { content    = c;  }

    // Member function 1: flip the read flag
    void markAsRead() { isRead = true; }

    // Member function 2: CONST FUNCTION – display message without modifying it
    void display() const {
        cout << "[" << timestamp << "]  "
             << "User#" << senderId << " → User#" << receiverId
             << " : " << content;
        if (!isRead) cout << "  [UNREAD]";
        cout << endl;
    }

    // Member function 3: check if this message was sent by a specific user
    bool isFromUser(int userId) const { return senderId == userId; }

    // Member function 4: STATIC – system-wide message counter
    static int getMessageCount() { return messageCount; }
};

int Message::messageCount = 0;

// ============================================================
//  CLASS 4 : Conversation
//  Purpose  : A thread of messages between buyer & seller.
//  Relation : COMPOSITION with Message (messages die with conversation)
//  Array    : messages[MAX_MESSAGES] – ARRAY OF OBJECTS (Req. 6)
// ============================================================
class Conversation {
private:
    static const int MAX_MESSAGES = 50;   // CONST – fixed array bound
    Message messages[MAX_MESSAGES];       // ARRAY OF OBJECTS – Requirement 6
    int  messageCount;
    int  buyerId;
    int  sellerId;
    int  listingId;
    bool isActive;

public:
    /*
     * Default constructor
     * Reason: Buyer stores Conversation objects in a fixed array,
     * which requires default-constructible elements.
     */
    Conversation()
        : messageCount(0), buyerId(0), sellerId(0),
          listingId(0), isActive(false) {}

    /*
     * Parameterised constructor
     * Reason: Used when a buyer intentionally starts a real conversation.
     */
    Conversation(int buyerId, int sellerId, int listingId)
        : messageCount(0), buyerId(buyerId), sellerId(sellerId),
          listingId(listingId), isActive(true) {}

    // --- Getters ---
    int  getBuyerId()      const { return buyerId;       }
    int  getSellerId()     const { return sellerId;      }
    int  getListingId()    const { return listingId;     }
    bool getIsActive()     const { return isActive;      }
    int  getMessageCount() const { return messageCount;  }

    // --- Setters ---
    void setBuyerId(int id)   { buyerId   = id; }
    void setSellerId(int id)  { sellerId  = id; }
    void setListingId(int id) { listingId = id; }

    // Member function 1: append a message to the array
    bool addMessage(const Message& m) {
        if (messageCount >= MAX_MESSAGES) {
            cout << "Conversation is full!" << endl;
            return false;
        }
        messages[messageCount++] = m;
        return true;
    }

    // Member function 2: display every message in the thread
    void displayAll() const {
        if (messageCount == 0) {
            cout << "No messages yet." << endl;
            return;
        }
        printLine();
        cout << "Conversation – Listing #" << listingId
             << "  (Buyer #" << buyerId
             << " ↔ Seller #" << sellerId << ")" << endl;
        printLine();
        for (int i = 0; i < messageCount; i++)
            messages[i].display();
    }

    // Member function 3: CONST FUNCTION – peek at last message
    Message getLastMessage() const {
        if (messageCount > 0) return messages[messageCount - 1];
        return Message();           // empty sentinel
    }

    // Member function 4: mark the conversation as closed
    void closeConversation() { isActive = false; }
};

// ============================================================
//  CLASS 5 : CarListing
//  Purpose  : A car listed on the marketplace.
//  Relation : COMPOSITION with Car (car details live and die
//             with the listing)
// ============================================================
class CarListing {
private:
    static int totalListings;   // STATIC – counts every listing ever created
                                // Why static? The marketplace needs a total
                                // count independent of any single listing.
    Car    car;                 // COMPOSITION – Car object is embedded here
    int    listingId;
    int    sellerId;
    int    askingPrice;
    ListingStatus status;
    string postedDate;
    int    viewCount;
    string description;

public:
    /*
     * Default constructor
     * Reason: Seller::listings[] is a fixed array; its elements must be
     * default-constructible before a real listing is assigned.
     */
    CarListing()
        : listingId(++totalListings), sellerId(0), askingPrice(0),
          status(ListingStatus::PENDING), postedDate(getCurrentDate()),
          viewCount(0), description("") {}

    /*
     * Parameterised constructor
     * Reason: Called when the seller actually posts a car with all data.
     */
    CarListing(const Car& car, int sellerId, int price, const string& description)
        : car(car), listingId(++totalListings), sellerId(sellerId),
          askingPrice(price), status(ListingStatus::PENDING),
          postedDate(getCurrentDate()), viewCount(0), description(description) {}

    /*
     * Copy constructor
     * Reason: When a listing is duplicated (e.g. re-listed) we copy all
     * data but assign a new unique listing id.
     */
    CarListing(const CarListing& o)
        : car(o.car), listingId(++totalListings), sellerId(o.sellerId),
          askingPrice(o.askingPrice), status(o.status),
          postedDate(o.postedDate), viewCount(o.viewCount),
          description(o.description) {}

    // Copy-assignment keeps the existing listingId (object already has identity)
    CarListing& operator=(const CarListing& o) {
        if (this != &o) {
            car         = o.car;
            sellerId    = o.sellerId;
            askingPrice = o.askingPrice;
            status      = o.status;
            postedDate  = o.postedDate;
            viewCount   = o.viewCount;
            description = o.description;
            // listingId intentionally not copied – object keeps its own id
        }
        return *this;
    }

    // --- Getters ---
    int           getListingId()   const { return listingId;   }
    int           getSellerId()    const { return sellerId;    }
    int           getAskingPrice() const { return askingPrice; }
    ListingStatus getStatus()      const { return status;      }
    string        getPostedDate()  const { return postedDate;  }
    int           getViewCount()   const { return viewCount;   }
    string        getDescription() const { return description; }
    Car           getCar()         const { return car;         }

    // --- Setters ---
    void setAskingPrice(int p)        { askingPrice = p; }
    void setStatus(ListingStatus s)   { status      = s; }
    void setDescription(const string& d) { description = d; }

    // Member function 1: approve this listing
    void approve() {
        status = ListingStatus::APPROVED;
        cout << "Listing #" << listingId << " approved!" << endl;
    }

    // Member function 2: flag as sold
    void markAsSold() {
        status = ListingStatus::SOLD;
        cout << "Listing #" << listingId << " marked as sold!" << endl;
    }

    // Member function 3: record a page view
    void incrementViews() { viewCount++; }

    // Member function 4: CONST FUNCTION – print all listing details
    void display() const {
        printLine();
        cout << "  Listing # : " << listingId
             << "  |  Status : " << statusToStr(status)      << endl;
        cout << "  Price     : Rs. " << askingPrice
             << "  |  Views  : " << viewCount               << endl;
        cout << "  Posted    : " << postedDate               << endl;
        cout << "  Desc      : " << description              << endl;
        car.display();
        printLine();
    }

    // STATIC FUNCTION – class-level info, not tied to any instance
    static int getTotalListings() { return totalListings; }
};

int CarListing::totalListings = 0;

// ============================================================
//  CLASS 6 : SearchFilter
//  Purpose  : Encapsulates all search/filter criteria.
//             Passed into Marketplace::searchListings().
// ============================================================
class SearchFilter {
private:
    string brand;
    string model;
    int    minPrice;
    int    maxPrice;
    int    minYear;
    int    maxYear;
    int    maxMileage;
    bool   hasCategoryFilter;
    Category category;

public:
    /*
     * Default constructor
     * Reason: A filter always starts with no restrictions (shows everything).
     * User then narrows it field by field.
     */
    SearchFilter()
        : brand(""), model(""), minPrice(0), maxPrice(999999999),
          minYear(1990), maxYear(2026), maxMileage(999999),
          hasCategoryFilter(false), category(Category::SEDAN) {}

    // --- Getters ---
    string getBrand()    const { return brand;    }
    string getModel()    const { return model;    }
    int getMinPrice()    const { return minPrice; }
    int getMaxPrice()    const { return maxPrice; }
    int getMinYear()     const { return minYear;  }
    int getMaxYear()     const { return maxYear;  }
    int getMaxMileage()  const { return maxMileage; }

    // --- Setters ---
    void setBrand(const string& b)   { brand    = b; }
    void setModel(const string& m)   { model    = m; }
    void setMinPrice(int p)          { minPrice = p; }
    void setMaxPrice(int p)          { maxPrice = p; }
    void setMinYear(int y)           { minYear  = y; }
    void setMaxYear(int y)           { maxYear  = y; }
    void setMaxMileage(int m)        { maxMileage = m; }
    void setCategory(Category c)     { hasCategoryFilter = true; category = c; }

    // Member function 1: CONST FUNCTION – check if a listing matches all filters
    // Only APPROVED listings appear in search results
    bool matches(const CarListing& listing) const {
        if (listing.getStatus() != ListingStatus::APPROVED)   return false;

        Car car = listing.getCar();

        if (!brand.empty() && car.getMake()  != brand)        return false;
        if (!model.empty() && car.getModel() != model)        return false;
        if (listing.getAskingPrice() < minPrice)              return false;
        if (listing.getAskingPrice() > maxPrice)              return false;
        if (car.getYear()    < minYear)                       return false;
        if (car.getYear()    > maxYear)                       return false;
        if (car.getMileage() > maxMileage)                    return false;
        if (hasCategoryFilter && car.getCategory() != category) return false;

        return true;
    }

    // Member function 2: clear all filters
    void reset() {
        brand    = "";   model     = "";
        minPrice = 0;    maxPrice  = 999999999;
        minYear  = 1990; maxYear   = 2026;
        maxMileage = 999999;
        hasCategoryFilter = false;
    }

    // Member function 3: CONST FUNCTION – print active filters
    void display() const {
        cout << "Active Filters:" << endl;
        if (!brand.empty())       cout << "  Brand     : " << brand     << endl;
        if (!model.empty())       cout << "  Model     : " << model     << endl;
        if (minPrice > 0)         cout << "  Min Price : Rs. " << minPrice << endl;
        if (maxPrice < 999999999) cout << "  Max Price : Rs. " << maxPrice << endl;
        if (minYear  > 1990)      cout << "  Min Year  : " << minYear   << endl;
        if (maxYear  < 2026)      cout << "  Max Year  : " << maxYear   << endl;
        if (maxMileage < 999999)  cout << "  Max KM    : " << maxMileage << endl;
    }

    // Member function 4: returns true if at least one filter has been set
    bool hasAnyFilter() const {
        return !brand.empty() || !model.empty() || minPrice > 0 ||
               maxPrice < 999999999 || minYear > 1990  || maxYear < 2026 ||
               maxMileage < 999999  || hasCategoryFilter;
    }
};

// ============================================================
//  CLASS 7 : User  (Base class)
//  Purpose  : Common identity data for every account type.
//  Relation : COMPOSITION with Address
//             Buyer, Seller, Admin inherit from User
// ============================================================
class User {
protected:
    const int userId;       // CONST MEMBER – ID assigned once, never changes
    static int userCount;   // STATIC – total registered users across all types
                            // Why static? Platform-wide metric, shared by all
                            // Buyer/Seller/Admin objects without preference.
    string name;
    string email;
    string password;
    long   phoneNum;
    Address address;        // COMPOSITION – address is a part-of User
    bool   isActive;

public:
    /*
     * Default constructor
     * Reason: Pointer arrays may need null/default-initialised Users.
     */
    User()
        : userId(++userCount), name(""), email(""), password(""),
          phoneNum(0), isActive(true) {}

    /*
     * Parameterised constructor
     * Reason: Creating a real account where all data is provided at once.
     */
    User(const string& name, const string& email, const string& password,
         long phone, const Address& address)
        : userId(++userCount), name(name), email(email), password(password),
          phoneNum(phone), address(address), isActive(true) {}

    // Virtual destructor – mandatory when deleting via base-class pointer
    virtual ~User() {}

    // --- Getters ---
    int     getUserId()   const { return userId;   }
    string  getName()     const { return name;     }
    string  getEmail()    const { return email;    }
    long    getPhoneNum() const { return phoneNum; }
    Address getAddress()  const { return address;  }
    bool    getIsActive() const { return isActive; }

    // --- Setters ---
    void setName(const string& n)     { name     = n; }
    void setEmail(const string& e)    { email    = e; }
    void setPassword(const string& p) { password = p; }
    void setPhoneNum(long p)          { phoneNum = p; }
    void setAddress(const Address& a) { address  = a; }
    void setIsActive(bool a)          { isActive = a; }

    // Member function 1: validate login credentials
    bool login(const string& inputEmail, const string& inputPass) const {
        return email == inputEmail && password == inputPass;
    }

    // Member function 2: update basic profile fields
    void updateProfile(const string& newName, long newPhone) {
        name     = newName;
        phoneNum = newPhone;
        cout << "Profile updated!" << endl;
    }

    // Member function 3: VIRTUAL CONST FUNCTION – display user info
    // Virtual so Buyer/Seller/Admin can extend it with their own fields
    virtual void display() const {
        cout << "  User ID   : " << userId   << endl;
        cout << "  Name      : " << name     << endl;
        cout << "  Email     : " << email    << endl;
        cout << "  Phone     : " << phoneNum << endl;
        cout << "  Address   : "; address.display(); cout << endl;
        cout << "  Status    : " << (isActive ? "Active" : "BANNED") << endl;
    }

    // Member function 4: STATIC – system-wide user count
    static int getTotalUsers() { return userCount; }
};

int User::userCount = 0;

// ============================================================
//  CLASS 8 : Seller  (extends User)
//  Purpose  : Posts and manages car listings.
//  Array    : listings[MAX_LISTINGS] – ARRAY OF OBJECTS (Req. 6)
// ============================================================
class Seller : public User {
private:
    static const int MAX_LISTINGS = 10; // CONST MEMBER – hard cap on active listings
    CarListing listings[MAX_LISTINGS];  // ARRAY OF OBJECTS – Requirement 6
    int   listingCount;
    float rating;       // average buyer rating (0.0 – 5.0)
    int   totalSales;

public:
    /*
     * Default constructor
     * Reason: Seller pointers may need default initialisation in vectors
     * before a real Seller is registered.
     */
    Seller()
        : User(), listingCount(0), rating(0.0f), totalSales(0) {}

    /*
     * Parameterised constructor
     * Reason: Used during registration when all account info is provided.
     */
    Seller(const string& name, const string& email, const string& password,
           long phone, const Address& address)
        : User(name, email, password, phone, address),
          listingCount(0), rating(0.0f), totalSales(0) {}

    // --- Getters ---
    int   getListingCount() const { return listingCount; }
    float getRating()       const { return rating;       }
    int   getTotalSales()   const { return totalSales;   }

    // --- Setters ---
    void setRating(float r) { rating = r; }

    // Member function 1: post a new listing (check cap first)
    bool postListing(const CarListing& listing) {
        if (listingCount >= MAX_LISTINGS) {
            cout << "Limit reached (" << MAX_LISTINGS
                 << " listings). Delete one to add more." << endl;
            return false;
        }
        listings[listingCount++] = listing;
        cout << "Listing posted and awaiting admin approval." << endl;
        return true;
    }

    // Member function 2: update price and description of an existing listing
    bool updateListing(int listingId, int newPrice, const string& newDesc) {
        for (int i = 0; i < listingCount; i++) {
            if (listings[i].getListingId() == listingId) {
                listings[i].setAskingPrice(newPrice);
                listings[i].setDescription(newDesc);
                cout << "Listing #" << listingId << " updated!" << endl;
                return true;
            }
        }
        cout << "Listing #" << listingId << " not found." << endl;
        return false;
    }

    // Member function 3: delete a listing (shift array to fill the gap)
    bool deleteListing(int listingId) {
        for (int i = 0; i < listingCount; i++) {
            if (listings[i].getListingId() == listingId) {
                for (int j = i; j < listingCount - 1; j++)
                    listings[j] = listings[j + 1];
                listingCount--;
                cout << "Listing #" << listingId << " deleted." << endl;
                return true;
            }
        }
        cout << "Listing #" << listingId << " not found." << endl;
        return false;
    }

    // Member function 4: CONST FUNCTION – show all seller's listings
    void viewMyListings() const {
        if (listingCount == 0) {
            cout << "You have no listings." << endl;
            return;
        }
        cout << "\n=== Your Listings ===" << endl;
        for (int i = 0; i < listingCount; i++)
            listings[i].display();
    }

    // Accessor used by Marketplace to sync the global pool
    CarListing& getListing(int index)             { return listings[index]; }
    const CarListing& getListing(int index) const { return listings[index]; }

    // display override adds seller-specific fields
    void display() const override {
        User::display();
        cout << "  Role      : Seller" << endl;
        cout << "  Rating    : " << rating << " / 5.0" << endl;
        cout << "  Sales     : " << totalSales << endl;
        cout << "  Listings  : " << listingCount << " / " << MAX_LISTINGS << endl;
    }
};

// ============================================================
//  CLASS 9 : Buyer  (extends User)
//  Purpose  : Browses listings, saves favourites, sends messages.
//  Relation : AGGREGATION with CarListing via favorites[]
//             (pointers – Buyer does not own the listings)
//  Array    : conversations[MAX_CONV] – ARRAY OF OBJECTS (Req. 6)
// ============================================================
class Buyer : public User {
private:
    static const int MAX_FAVORITES     = 20;
    static const int MAX_CONVERSATIONS = 10;

    /*
     * AGGREGATION – Buyer stores POINTERS to CarListing objects.
     * The listings are owned by Seller and exist in Marketplace's
     * global pool. If the Buyer is deleted the listings survive.
     * We store const pointers because the buyer should not modify
     * another seller's listing.
     */
    const CarListing* favorites[MAX_FAVORITES];
    int favoritesCount;

    // ARRAY OF OBJECTS – Buyer owns its conversations (Requirement 6)
    Conversation conversations[MAX_CONVERSATIONS];
    int conversationCount;

    string memberSince;

public:
    /*
     * Default constructor
     * Reason: Buyer pointers in Marketplace vectors may be
     * default-initialised before registration data is available.
     */
    Buyer()
        : User(), favoritesCount(0), conversationCount(0),
          memberSince(getCurrentDate()) {
        for (int i = 0; i < MAX_FAVORITES; i++) favorites[i] = nullptr;
    }

    /*
     * Parameterised constructor
     * Reason: Called at registration with full account data.
     */
    Buyer(const string& name, const string& email, const string& password,
          long phone, const Address& address)
        : User(name, email, password, phone, address),
          favoritesCount(0), conversationCount(0),
          memberSince(getCurrentDate()) {
        for (int i = 0; i < MAX_FAVORITES; i++) favorites[i] = nullptr;
    }

    // --- Getters ---
    int    getFavoritesCount()    const { return favoritesCount;    }
    int    getConversationCount() const { return conversationCount; }
    string getMemberSince()       const { return memberSince;       }

    // Member function 1: save a pointer to a listing (AGGREGATION – no ownership)
    bool addToFavorites(const CarListing* listing) {
        if (favoritesCount >= MAX_FAVORITES) {
            cout << "Favourites list is full!" << endl;
            return false;
        }
        for (int i = 0; i < favoritesCount; i++) {
            if (favorites[i]->getListingId() == listing->getListingId()) {
                cout << "Already in favourites." << endl;
                return false;
            }
        }
        favorites[favoritesCount++] = listing;
        cout << "Added to favourites!" << endl;
        return true;
    }

    // Member function 2: drop a pointer from favourites (listing still exists)
    bool removeFavorite(int listingId) {
        for (int i = 0; i < favoritesCount; i++) {
            if (favorites[i]->getListingId() == listingId) {
                for (int j = i; j < favoritesCount - 1; j++)
                    favorites[j] = favorites[j + 1];
                favorites[--favoritesCount] = nullptr;
                cout << "Removed from favourites." << endl;
                return true;
            }
        }
        cout << "Listing not found in favourites." << endl;
        return false;
    }

    // Member function 3: CONST FUNCTION – print all saved favourites
    void viewFavorites() const {
        if (favoritesCount == 0) {
            cout << "No favourites saved." << endl;
            return;
        }
        cout << "\n=== Your Favourites ===" << endl;
        for (int i = 0; i < favoritesCount; i++)
            favorites[i]->display();
    }

    // Member function 4: send a message to a seller about a specific listing
    // Finds an existing conversation or creates a new one
    bool sendMessage(int sellerId, int listingId, const string& content) {
        // Check for existing conversation with same seller + listing
        for (int i = 0; i < conversationCount; i++) {
            if (conversations[i].getSellerId()  == sellerId &&
                conversations[i].getListingId() == listingId) {
                Message msg(userId, sellerId, content);
                bool ok = conversations[i].addMessage(msg);
                if (ok) cout << "Message sent!" << endl;
                return ok;
            }
        }
        // No existing thread – open a new conversation
        if (conversationCount >= MAX_CONVERSATIONS) {
            cout << "Max conversations reached!" << endl;
            return false;
        }
        conversations[conversationCount] = Conversation(userId, sellerId, listingId);
        Message msg(userId, sellerId, content);
        conversations[conversationCount].addMessage(msg);
        conversationCount++;
        cout << "New conversation started. Message sent!" << endl;
        return true;
    }

    // CONST FUNCTION – print all conversation threads
    void viewConversations() const {
        if (conversationCount == 0) {
            cout << "No conversations yet." << endl;
            return;
        }
        for (int i = 0; i < conversationCount; i++)
            conversations[i].displayAll();
    }

    // display override
    void display() const override {
        User::display();
        cout << "  Role      : Buyer" << endl;
        cout << "  Member    : " << memberSince << endl;
        cout << "  Favourites: " << favoritesCount
             << " / " << MAX_FAVORITES << endl;
    }
};

// ============================================================
//  CLASS 10 : Admin  (extends User)
//  Purpose   : Moderates the platform.
// ============================================================
class Admin : public User {
private:
    string adminLevel;        // "junior" or "senior"
    int    listingsApproved;
    int    listingsRemoved;
    static int totalAdmins;   // STATIC – how many Admins currently exist
                              // Why static? HR/system metric; the count
                              // belongs to the class, not any one Admin.

public:
    /*
     * Default constructor
     * Reason: Admin pointers may need to be initialised before
     * an actual Admin account is created.
     */
    Admin()
        : User(), adminLevel("junior"),
          listingsApproved(0), listingsRemoved(0) { totalAdmins++; }

    /*
     * Parameterised constructor
     * Reason: Used at registration with all required fields known.
     * Default argument for level so caller can omit it.
     */
    Admin(const string& name, const string& email, const string& password,
          long phone, const Address& address, const string& level = "junior")
        : User(name, email, password, phone, address),
          adminLevel(level), listingsApproved(0), listingsRemoved(0) {
        totalAdmins++;
    }

    ~Admin() { totalAdmins--; }  // Decrease count when admin is destroyed

    // --- Getters ---
    string getAdminLevel()        const { return adminLevel;        }
    int    getListingsApproved()  const { return listingsApproved;  }
    int    getListingsRemoved()   const { return listingsRemoved;   }

    // --- Setters ---
    void setAdminLevel(const string& l) { adminLevel = l; }

    // Member function 1: approve a pending listing
    void approveListing(CarListing& listing) {
        listing.approve();
        listingsApproved++;
    }

    // Member function 2: remove a listing from the marketplace
    void removeListing(CarListing& listing) {
        listing.setStatus(ListingStatus::REMOVED);
        listingsRemoved++;
        cout << "Listing #" << listing.getListingId() << " removed." << endl;
    }

    // Member function 3: ban a user account (works for any User subtype)
    void banUser(User& user) {
        user.setIsActive(false);
        cout << "User #" << user.getUserId()
             << " (" << user.getName() << ") has been banned." << endl;
    }

    // Member function 4: display all PENDING listings from the global pool
    void viewPendingListings(const vector<CarListing*>& pool) const {
        bool found = false;
        cout << "\n=== Pending Listings ===" << endl;
        for (auto l : pool) {
            if (l->getStatus() == ListingStatus::PENDING) {
                l->display();
                found = true;
            }
        }
        if (!found) cout << "No pending listings." << endl;
    }

    // STATIC FUNCTION
    static int getTotalAdmins() { return totalAdmins; }

    // display override
    void display() const override {
        User::display();
        cout << "  Role      : Admin (" << adminLevel << ")" << endl;
        cout << "  Approved  : " << listingsApproved << endl;
        cout << "  Removed   : " << listingsRemoved  << endl;
    }
};

int Admin::totalAdmins = 0;

// ============================================================
//  CLASS 11 : Marketplace
//  Purpose  : Central controller – ties all classes together.
//  Relation : AGGREGATION with Seller*, Buyer*, Admin*
//             (objects are heap-allocated; Marketplace manages their
//             lifetime, but conceptually users exist independently)
// ============================================================
class Marketplace {
private:
    const string PLATFORM_NAME;  // CONST MEMBER – brand name never changes
    static int sessionCount;     // STATIC – counts app launches
                                 // Why static? Persists across any number of
                                 // Marketplace objects in one process run.

    // AGGREGATION – pointers to heap-allocated user objects
    vector<Seller*> sellers;
    vector<Buyer*>  buyers;
    vector<Admin*>  admins;

    // Global listing pool: pointers to listings stored inside each Seller
    // This is also AGGREGATION – Marketplace doesn't own the CarListing objects
    vector<CarListing*> allListings;

    User*  loggedInUser;     // pointer to whichever user is currently logged in
    string loggedInRole;     // "seller" | "buyer" | "admin"

    // ---- PRIVATE HELPERS ----

    // Rebuild the flat listing pool from every seller's listings array
    void syncListings() {
        allListings.clear();
        for (auto s : sellers) {
            for (int i = 0; i < s->getListingCount(); i++)
                allListings.push_back(&s->getListing(i));
        }
    }

    // Build a Car object from console input
    Car buildCarFromInput() {
        string make, model, color;
        int year, mileage, engineCC, catChoice, fuelChoice;

        cout << "Make        : "; cin >> make;
        cout << "Model       : "; cin >> model;
        cout << "Year        : "; cin >> year;
        cout << "Mileage(km) : "; cin >> mileage;
        cout << "Engine CC   : "; cin >> engineCC;
        cout << "Color       : "; cin >> color;
        cout << "Category    (1=Sedan 2=SUV 3=Van 4=Hatchback 5=Truck): ";
        cin >> catChoice;
        cout << "Fuel type   (1=Petrol 2=Diesel 3=Hybrid 4=Electric)  : ";
        cin >> fuelChoice;

        // Clamp enum index to valid range
        if (catChoice  < 1 || catChoice  > 5) catChoice  = 1;
        if (fuelChoice < 1 || fuelChoice > 4) fuelChoice = 1;

        return Car(make, model, year, mileage, engineCC, color,
                   static_cast<Category>(catChoice - 1),
                   static_cast<FuelType>(fuelChoice - 1));
    }

    // ---- REGISTRATION HELPERS ----

    void registerSeller() {
        string name, email, password, city, area;
        long phone;
        cout << "\n--- Seller Registration ---" << endl;
        cout << "Name    : "; cin.ignore(); getline(cin, name);
        cout << "Email   : "; cin >> email;
        cout << "Password: "; cin >> password;
        cout << "Phone   : "; cin >> phone;
        cout << "City    : "; cin >> city;
        cout << "Area    : "; cin >> area;

        Address addr(city, area, "Sindh", 75000);
        Seller* s = new Seller(name, email, password, phone, addr);
        sellers.push_back(s);
        cout << "Registered! Your Seller ID: " << s->getUserId() << endl;
    }

    void registerBuyer() {
        string name, email, password, city, area;
        long phone;
        cout << "\n--- Buyer Registration ---" << endl;
        cout << "Name    : "; cin.ignore(); getline(cin, name);
        cout << "Email   : "; cin >> email;
        cout << "Password: "; cin >> password;
        cout << "Phone   : "; cin >> phone;
        cout << "City    : "; cin >> city;
        cout << "Area    : "; cin >> area;

        Address addr(city, area, "Sindh", 75000);
        Buyer* b = new Buyer(name, email, password, phone, addr);
        buyers.push_back(b);
        cout << "Registered! Your Buyer ID: " << b->getUserId() << endl;
    }

    void registerAdmin() {
        string name, email, password;
        long phone;
        cout << "\n--- Admin Registration ---" << endl;
        cout << "Name    : "; cin.ignore(); getline(cin, name);
        cout << "Email   : "; cin >> email;
        cout << "Password: "; cin >> password;
        cout << "Phone   : "; cin >> phone;

        Address addr("Karachi", "FAST Campus", "Sindh", 75080);
        Admin* a = new Admin(name, email, password, phone, addr, "senior");
        admins.push_back(a);
        cout << "Registered! Your Admin ID: " << a->getUserId() << endl;
    }

    // ---- LOGIN ----

    bool loginUser() {
        string role, email, password;
        cout << "\nRole (buyer / seller / admin): "; cin >> role;
        cout << "Email   : "; cin >> email;
        cout << "Password: "; cin >> password;

        if (role == "seller") {
            for (auto s : sellers) {
                if (s->login(email, password) && s->getIsActive()) {
                    loggedInUser = s;
                    loggedInRole = "seller";
                    cout << "Welcome back, " << s->getName() << "!" << endl;
                    return true;
                }
            }
        } else if (role == "buyer") {
            for (auto b : buyers) {
                if (b->login(email, password) && b->getIsActive()) {
                    loggedInUser = b;
                    loggedInRole = "buyer";
                    cout << "Welcome back, " << b->getName() << "!" << endl;
                    return true;
                }
            }
        } else if (role == "admin") {
            for (auto a : admins) {
                if (a->login(email, password)) {
                    loggedInUser = a;
                    loggedInRole = "admin";
                    cout << "Welcome, Admin " << a->getName() << "!" << endl;
                    return true;
                }
            }
        }
        cout << "Invalid credentials or account banned." << endl;
        return false;
    }

    void doLogout() {
        loggedInUser = nullptr;
        loggedInRole = "";
        cout << "Logged out successfully." << endl;
    }

    // ---- SELLER MENU ----

    void sellerMenu() {
        Seller* seller = dynamic_cast<Seller*>(loggedInUser);
        if (!seller) return;

        while (true) {
            cout << "\n=== Seller Menu ===" << endl;
            cout << "1. Post a Car Listing"     << endl;
            cout << "2. View My Listings"        << endl;
            cout << "3. Update a Listing"        << endl;
            cout << "4. Delete a Listing"        << endl;
            cout << "5. View My Profile"         << endl;
            cout << "6. Update Profile"          << endl;
            cout << "7. Logout"                  << endl;
            cout << "Choice: ";
            int c; cin >> c;

            if (c == 1) {
                cout << "\n--- New Car Listing ---" << endl;
                Car car = buildCarFromInput();
                int price;
                string desc;
                cout << "Asking Price (Rs): "; cin >> price;
                cout << "Description      : "; cin.ignore(); getline(cin, desc);
                CarListing listing(car, seller->getUserId(), price, desc);
                seller->postListing(listing);
                syncListings();

            } else if (c == 2) {
                seller->viewMyListings();

            } else if (c == 3) {
                seller->viewMyListings();
                cout << "Listing ID to update: "; int id;    cin >> id;
                cout << "New Price            : "; int price; cin >> price;
                cout << "New Description      : "; string desc;
                cin.ignore(); getline(cin, desc);
                seller->updateListing(id, price, desc);

            } else if (c == 4) {
                seller->viewMyListings();
                cout << "Listing ID to delete: "; int id; cin >> id;
                seller->deleteListing(id);
                syncListings();

            } else if (c == 5) {
                seller->display();

            } else if (c == 6) {
                string newName; long newPhone;
                cout << "New Name : "; cin.ignore(); getline(cin, newName);
                cout << "New Phone: "; cin >> newPhone;
                seller->updateProfile(newName, newPhone);

            } else if (c == 7) {
                doLogout(); break;

            } else {
                cout << "Invalid choice." << endl;
            }
        }
    }

    // ---- BUYER MENU ----

    void buyerMenu() {
        Buyer* buyer = dynamic_cast<Buyer*>(loggedInUser);
        if (!buyer) return;

        while (true) {
            cout << "\n=== Buyer Menu ===" << endl;
            cout << "1. Browse All Approved Listings" << endl;
            cout << "2. Search / Filter Listings"     << endl;
            cout << "3. Add to Favourites"            << endl;
            cout << "4. Remove from Favourites"       << endl;
            cout << "5. View Favourites"              << endl;
            cout << "6. Send Message to Seller"       << endl;
            cout << "7. View My Conversations"        << endl;
            cout << "8. View My Profile"              << endl;
            cout << "9. Logout"                       << endl;
            cout << "Choice: ";
            int c; cin >> c;

            syncListings();

            if (c == 1) {
                bool found = false;
                for (auto l : allListings) {
                    if (l->getStatus() == ListingStatus::APPROVED) {
                        l->display();
                        l->incrementViews();
                        found = true;
                    }
                }
                if (!found) cout << "No approved listings available." << endl;

            } else if (c == 2) {
                SearchFilter filter;
                string brand;
                int val;
                cout << "\n--- Set Filters (enter 0 to skip numeric fields) ---"
                     << endl;
                cout << "Brand (0=skip): "; cin >> brand;
                if (brand != "0") filter.setBrand(brand);

                cout << "Min Price (0=skip): "; cin >> val;
                if (val > 0) filter.setMinPrice(val);

                cout << "Max Price (0=skip): "; cin >> val;
                if (val > 0) filter.setMaxPrice(val);

                cout << "Min Year (0=skip): "; cin >> val;
                if (val > 0) filter.setMinYear(val);

                cout << "Max Year (0=skip): "; cin >> val;
                if (val > 0) filter.setMaxYear(val);

                cout << "Max Mileage km (0=skip): "; cin >> val;
                if (val > 0) filter.setMaxMileage(val);

                if (filter.hasAnyFilter()) filter.display();

                bool found = false;
                cout << "\n=== Search Results ===" << endl;
                for (auto l : allListings) {
                    if (filter.matches(*l)) {
                        l->display();
                        l->incrementViews();
                        found = true;
                    }
                }
                if (!found) cout << "No listings match your criteria." << endl;

            } else if (c == 3) {
                cout << "Listing ID to save: "; int id; cin >> id;
                bool found = false;
                for (auto l : allListings) {
                    if (l->getListingId() == id) {
                        buyer->addToFavorites(l);
                        found = true;
                        break;
                    }
                }
                if (!found) cout << "Listing #" << id << " not found." << endl;

            } else if (c == 4) {
                cout << "Listing ID to remove from favourites: ";
                int id; cin >> id;
                buyer->removeFavorite(id);

            } else if (c == 5) {
                buyer->viewFavorites();

            } else if (c == 6) {
                cout << "Seller User ID : "; int sid; cin >> sid;
                cout << "Listing ID     : "; int lid; cin >> lid;
                cout << "Message        : "; string msg;
                cin.ignore(); getline(cin, msg);
                buyer->sendMessage(sid, lid, msg);

            } else if (c == 7) {
                buyer->viewConversations();

            } else if (c == 8) {
                buyer->display();

            } else if (c == 9) {
                doLogout(); break;

            } else {
                cout << "Invalid choice." << endl;
            }
        }
    }

    // ---- ADMIN MENU ----

    void adminMenu() {
        Admin* admin = dynamic_cast<Admin*>(loggedInUser);
        if (!admin) return;

        while (true) {
            cout << "\n=== Admin Menu ===" << endl;
            cout << "1. View All Listings"       << endl;
            cout << "2. View Pending Listings"   << endl;
            cout << "3. Approve a Listing"       << endl;
            cout << "4. Remove a Listing"        << endl;
            cout << "5. Ban a User"              << endl;
            cout << "6. System Statistics"       << endl;
            cout << "7. Logout"                  << endl;
            cout << "Choice: ";
            int c; cin >> c;

            syncListings();

            if (c == 1) {
                if (allListings.empty()) cout << "No listings." << endl;
                for (auto l : allListings) l->display();

            } else if (c == 2) {
                admin->viewPendingListings(allListings);

            } else if (c == 3) {
                cout << "Listing ID to approve: "; int id; cin >> id;
                bool found = false;
                for (auto l : allListings) {
                    if (l->getListingId() == id) {
                        admin->approveListing(*l);
                        found = true; break;
                    }
                }
                if (!found) cout << "Listing #" << id << " not found." << endl;

            } else if (c == 4) {
                cout << "Listing ID to remove: "; int id; cin >> id;
                bool found = false;
                for (auto l : allListings) {
                    if (l->getListingId() == id) {
                        admin->removeListing(*l);
                        found = true; break;
                    }
                }
                if (!found) cout << "Listing #" << id << " not found." << endl;

            } else if (c == 5) {
                string role;
                int uid;
                cout << "Role of user to ban (buyer / seller): "; cin >> role;
                cout << "User ID: "; cin >> uid;
                bool found = false;
                if (role == "buyer") {
                    for (auto b : buyers) {
                        if (b->getUserId() == uid) {
                            admin->banUser(*b); found = true; break;
                        }
                    }
                } else {
                    for (auto s : sellers) {
                        if (s->getUserId() == uid) {
                            admin->banUser(*s); found = true; break;
                        }
                    }
                }
                if (!found) cout << "User not found." << endl;

            } else if (c == 6) {
                printLine();
                cout << "System Statistics" << endl;
                printLine();
                cout << "Total Registered Users : " << User::getTotalUsers()          << endl;
                cout << "Total Listings Created : " << CarListing::getTotalListings()  << endl;
                cout << "Total Car Objects Made : " << Car::getCarsCount()             << endl;
                cout << "Total Messages Sent    : " << Message::getMessageCount()      << endl;
                cout << "Total Admins           : " << Admin::getTotalAdmins()         << endl;
                cout << "Active Sellers         : " << sellers.size()                  << endl;
                cout << "Active Buyers          : " << buyers.size()                   << endl;
                cout << "Session Number         : " << sessionCount                    << endl;
                printLine();

            } else if (c == 7) {
                doLogout(); break;

            } else {
                cout << "Invalid choice." << endl;
            }
        }
    }

public:
    /*
     * Constructor
     * Reason: Marketplace is always constructed with its name fixed.
     * No default constructor needed – there is exactly one marketplace.
     */
    Marketplace()
        : PLATFORM_NAME("PakWheels Clone"), loggedInUser(nullptr) {
        sessionCount++;
        cout << "=============================" << endl;
        cout << "   " << PLATFORM_NAME         << endl;
        cout << "   Session #" << sessionCount  << endl;
        cout << "=============================" << endl;
    }

    // Destructor frees all heap-allocated user objects
    ~Marketplace() {
        for (auto s : sellers) delete s;
        for (auto b : buyers)  delete b;
        for (auto a : admins)  delete a;
    }

    // STATIC FUNCTION
    static int getSessionCount() { return sessionCount; }

    // Main application loop
    void startLoop() {
        while (true) {
            cout << "\n=============================\n";
            cout << "  " << PLATFORM_NAME << "\n";
            cout << "=============================\n";
            cout << "1. Register as Seller\n";
            cout << "2. Register as Buyer\n";
            cout << "3. Register as Admin\n";
            cout << "4. Login\n";
            cout << "5. Exit\n";
            cout << "Choice: ";
            int c; cin >> c;

            if      (c == 1) registerSeller();
            else if (c == 2) registerBuyer();
            else if (c == 3) registerAdmin();
            else if (c == 4) {
                if (loginUser()) {
                    if      (loggedInRole == "seller") sellerMenu();
                    else if (loggedInRole == "buyer")  buyerMenu();
                    else if (loggedInRole == "admin")  adminMenu();
                }
            }
            else if (c == 5) { cout << "Goodbye!\n"; break; }
            else              cout << "Invalid choice.\n";
        }
    }
};

int Marketplace::sessionCount = 0;

// ============================================================
//  MAIN
// ============================================================
int main() {
    Marketplace mp;
    mp.startLoop();
    return 0;
}